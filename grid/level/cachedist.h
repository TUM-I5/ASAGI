/**
 * @file
 *  This file is part of ASAGI.
 *
 *  ASAGI is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as
 *  published by the Free Software Foundation, either version 3 of
 *  the License, or  (at your option) any later version.
 *
 *  ASAGI is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with ASAGI.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Diese Datei ist Teil von ASAGI.
 *
 *  ASAGI ist Freie Software: Sie koennen es unter den Bedingungen
 *  der GNU Lesser General Public License, wie von der Free Software
 *  Foundation, Version 3 der Lizenz oder (nach Ihrer Option) jeder
 *  spaeteren veroeffentlichten Version, weiterverbreiten und/oder
 *  modifizieren.
 *
 *  ASAGI wird in der Hoffnung, dass es nuetzlich sein wird, aber
 *  OHNE JEDE GEWAEHELEISTUNG, bereitgestellt; sogar ohne die implizite
 *  Gewaehrleistung der MARKTFAEHIGKEIT oder EIGNUNG FUER EINEN BESTIMMTEN
 *  ZWECK. Siehe die GNU Lesser General Public License fuer weitere Details.
 *
 *  Sie sollten eine Kopie der GNU Lesser General Public License zusammen
 *  mit diesem Programm erhalten haben. Wenn nicht, siehe
 *  <http://www.gnu.org/licenses/>.
 * 
 * @copyright 2015 Sebastian Rettenberger <rettenbs@in.tum.de>
 */

#ifndef GRID_LEVEL_CACHEDIST_H
#define GRID_LEVEL_CACHEDIST_H

#include <cassert>

#include "cache.h"
#include "allocator/mpialloc.h"
#include "transfer/numacache.h"
#include "transfer/numano.h"
#ifdef USE_MPI3
#include "transfer/mpiwincache.h"
#endif // USE_MPI3
#include "transfer/mpithreadcache.h"
#include "transfer/mpino.h"

namespace grid
{

namespace level
{

/**
 * Only caches blocks.
 * Blocks not in the local cache will be fetched from other caches
 * or the file.
 */
template<class MPITrans, class NumaTrans, class Type, class Allocator>
class CacheDist : public Cache<Type, Allocator>
{
private:
	/** The MPI transfer class */
	MPITrans m_mpiTrans;

	/** The NUMA transfer class */
	NumaTrans m_numaTrans;

	/** Number of blocks in the cache */
	unsigned int m_cacheSize;

public:
	/**
	 * @copydoc Cache::Cache
	 */
	CacheDist(mpi::MPIComm &comm,
			const numa::Numa &numa,
			Type &type)
		: Cache<Type, Allocator>(comm, numa, type),
		  m_cacheSize(0)
	{
	}

	virtual ~CacheDist()
	{
	}

	/**
	 * @copydoc Cache::open
	 */
	asagi::Grid::Error open(
		const char* filename,
		const char* varname,
		const int* blockSize,
		int timeDimension,
		unsigned int cacheSize,
		int cacheHandSpread,
		grid::ValuePosition valuePos)
	{
		asagi::Grid::Error err = Cache<Type, Allocator>::open(
				filename, varname,
				blockSize, timeDimension,
				cacheSize, cacheHandSpread,
				valuePos);
		if (err != asagi::Grid::SUCCESS)
			return err;

		m_cacheSize = cacheSize;

		// Initialize the MPI transfer class
		err = m_mpiTrans.init(this->cache(), cacheSize, this->cacheManager(),
				this->localBlockCount(), this->totalBlockSize(),
				this->type(), this->comm(), this->numa());
		if (err != asagi::Grid::SUCCESS)
			return err;

		// Initialize the NUMA transfer class
		err = m_numaTrans.init(this->totalBlockSize(),
				this->type(), this->numa(), this->cacheManager());
		if (err != asagi::Grid::SUCCESS)
			return err;

		return asagi::Grid::SUCCESS;
	}

	/**
	 * @copydoc Cache::getAt
	 */
	template<typename T>
	void getAt(T* buf, const double* pos)
	{
		this->incCounter(perf::Counter::ACCESS);

		// Get the index from the position
		size_t index[MAX_DIMENSIONS];
		this->pos2index(pos, index);

		// Get block id from the index
		unsigned long globalBlockId = this->blockByCoords(index);

		unsigned long cacheOffset;
		unsigned char* data;
		long oldGlobalBlockId = this->cacheManager().get(globalBlockId, cacheOffset, data);

		assert(cacheOffset < m_cacheSize);

		if (static_cast<long>(globalBlockId) != oldGlobalBlockId) {
			// Delete the old block from the dictionary
			m_mpiTrans.deleteBlock(oldGlobalBlockId,
					this->blockRank(oldGlobalBlockId),
					this->blockNodeOffset(oldGlobalBlockId),
					cacheOffset + m_cacheSize * this->numaDomainId());

			// Try NUMA first
			if (m_numaTrans.transfer(globalBlockId, data)) {
				this->incCounter(perf::Counter::NUMA);

				m_mpiTrans.addBlock(globalBlockId,
						this->blockRank(globalBlockId),
						this->blockNodeOffset(globalBlockId),
						cacheOffset + m_cacheSize * this->numaDomainId());
			} else {
				// MPI windows have collision avoidance, because the block will be locked
				// during transfer and cannot be deleted.
				// The communication thread uses error detection. Which means, a block
				// might get deleted between startTransfer and transfer. In this case,
				// retrying another MPI rank might be worthwhile.

				bool retry;
				bool received = false;

				do {
					retry = false;

					// Get the cache entry
					long entry = m_mpiTrans.startTransfer(globalBlockId,
							this->blockRank(globalBlockId),
							this->blockNodeOffset(globalBlockId),
							cacheOffset + m_cacheSize * this->numaDomainId());

					// Fill the cache
					if (m_mpiTrans.transfer(entry, globalBlockId, data, retry)) {
						this->incCounter(perf::Counter::MPI);
						received = true;
					}
				} while (retry);

				if (!received)
					this->loadBlock(index, data);

				// Finalize the communication
				m_mpiTrans.endTransfer(globalBlockId,
						this->blockRank(globalBlockId),
						this->blockNodeOffset(globalBlockId),
						cacheOffset + m_cacheSize * this->numaDomainId());
			}
		}

		// Compute the offset in the block
		unsigned long offset = this->calcOffsetInBlock(index);

		assert(offset < this->totalBlockSize());

		// Finally, we fill the buffer
		this->type().convert(&data[this->typeSize() * offset], buf);

		// Free the block in the cache
		this->cacheManager().unlock(cacheOffset);
	}
};

/** Cached distributed level with NUMA */
template<class Type>
using CacheDistNuma = CacheDist<transfer::MPINo, transfer::NumaCache, Type, allocator::Default>;

/** Cached distributed level with MPI (communication thread) */
template<class Type>
using CacheDistMPIThread = CacheDist<transfer::MPIThreadCache, transfer::NumaNo, Type, allocator::MPIAlloc>;

#ifdef USE_MPI3
/** Cached distributed level with MPI windows */
template<class Type>
using CacheDistMPIWin = CacheDist<transfer::MPIWinCache, transfer::NumaNo, Type, allocator::MPIAlloc>;
#endif // USE_MPI3

/** Cached distributed level with MPI (communication thread) and NUMA */
template<class Type>
using CacheDistMPIThreadNuma = CacheDist<transfer::MPIThreadCache, transfer::NumaCache, Type, allocator::MPIAlloc>;

#ifdef USE_MPI3
/** Cached distributed level with MPI windows and NUMA */
template<class Type>
using CacheDistMPIWinNuma = CacheDist<transfer::MPIWinCache, transfer::NumaCache, Type, allocator::MPIAlloc>;
#endif // USE_MPI3

}

}

#endif /* GRID_LEVEL_CACHEDIST_H */
