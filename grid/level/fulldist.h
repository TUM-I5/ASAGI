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
 * @copyright 2012-2015 Sebastian Rettenberger <rettenbs@in.tum.de>
 */

#ifndef GRID_LEVEL_FULLDIST_H
#define GRID_LEVEL_FULLDIST_H

#include "full.h"
#include "allocator/default.h"
#include "allocator/mpialloc.h"
#include "cache/cachemanager.h"

namespace grid
{

namespace level
{

/**
 * Grid level implementation, that distributes the grid at the beginning
 * across all MPI tasks. If a block is not available, it is transfered via
 * MPI and stored in a cache.
 */
template<class MPITrans, class NumaTrans, class Type, class Allocator>
class FullDist : public Full<MPITrans, NumaTrans, Type, Allocator>
{
private:
	/** Manager used to control the cache */
	cache::CacheManager<allocator::Default> m_cacheManager;

	/** The MPI transfer class */
	MPITrans m_mpiTrans;

	/** The NUMA transfer class */
	NumaTrans m_numaTrans;

public:
	FullDist(const FullDist<MPITrans, NumaTrans, Type, Allocator> &other)
		: Full<MPITrans, NumaTrans, Type, Allocator>(other),
		  m_cacheManager(other.m_cacheManager),
		  m_mpiTrans(other.m_mpiTrans), m_numaTrans(other.m_numaTrans)
	{
	}

	FullDist(const mpi::MPIComm &comm,
			const numa::Numa &numa,
			Type &type)
		: Full<MPITrans, NumaTrans, Type, Allocator>(comm, numa, type)
	{
	}

	virtual ~FullDist()
	{
	}

	asagi::Grid::Error open(
		const char* filename,
		const char* varname,
		const unsigned int* blockSize,
		int timeDimension,
		unsigned int cacheSize,
		int cacheHandSpread,
		grid::ValuePosition valuePos)
	{
		asagi::Grid::Error err = Full<MPITrans, NumaTrans, Type, Allocator>::open(
				filename, varname,
				blockSize, timeDimension,
				cacheSize, cacheHandSpread,
				valuePos);
		if (err != asagi::Grid::SUCCESS)
			return err;

		// Initialize the cache manager
		err = m_cacheManager.init(cacheSize,
				this->typeSize() * this->totalBlockSize(),
				cacheHandSpread);
		if (err != asagi::Grid::SUCCESS)
			return err;

		// Initialize the MPI transfer class
		err = m_mpiTrans.init(this->data(),
				this->localBlockCount(), this->totalBlockSize(),
				this->type(), this->comm(), this->numa());
		if (err != asagi::Grid::SUCCESS)
			return err;

		// Initialize NUMA transfer class
		err = m_numaTrans.init(this->data(),
				this->localBlockCount(), this->totalBlockSize(),
				this->type(), this->comm(), this->numa(),
				m_cacheManager);
		if (err != asagi::Grid::SUCCESS)
			return err;

		return asagi::Grid::SUCCESS;
	}

	template<typename T>
	void getAt(T* buf, const double* pos)
	{
		// Get the index from the position
		size_t index[MAX_DIMENSIONS];
		this->pos2index(pos, index);

		// Get block id from the index
		unsigned long globalBlockId = this->blockByCoords(index);

		if (this->blockRank(globalBlockId) == this->comm().rank()
				&& this->blockDomain(globalBlockId) == this->numaDomainId()) {
			Full<MPITrans, NumaTrans, Type, Allocator>::getAt(buf, pos);
			return;
		}

		// Only increment this if the case is not handled by the parent class
		this->incCounter(perf::Counter::ACCESS);

		// Check the cache
		unsigned long cacheId;
		unsigned char* cache;
		long oldGlobalBlockId = m_cacheManager.get(globalBlockId, cacheId, cache);

		if (static_cast<long>(globalBlockId) != oldGlobalBlockId) {
			// Cache not filled, do this first

			if (m_numaTrans.transfer(globalBlockId,
					this->blockRank(globalBlockId), this->blockDomain(globalBlockId),
					this->blockOffset(globalBlockId), cache)) {
				this->incCounter(perf::Counter::NUMA);
			} else {
				this->incCounter(perf::Counter::MPI);

				m_mpiTrans.transfer(this->blockRank(globalBlockId),
						this->blockNodeOffset(globalBlockId),
						cache);
			}
		}

		// Compute the offset in the block
		unsigned long offset = this->calcOffsetInBlock(index);

		assert(offset < this->totalBlockSize());

		// Finally, we fill the buffer
		this->type().convert(&cache[this->typeSize() * offset], buf);

		// Free the block in the cache
		m_cacheManager.unlock(cacheId);
	}

#if 0
public:
	DistStaticGrid(const GridContainer &container,
		unsigned int hint = asagi::Grid::NO_HINT);
	virtual ~DistStaticGrid();
	
protected:
	asagi::Grid::Error init();
	
	void getAt(void* buf, types::Type::converter_t converter,
		unsigned long x, unsigned long y = 0, unsigned long z = 0);

	void getBlock(unsigned long block,
		long oldBlock,
		unsigned long cacheIndex,
		unsigned char* cache);

	/**
	 * We can free all netCDF related resources, because we use MPI to
	 * transfer blocks
	 */
	bool keepFileOpen() const
	{
		return false;
	}
#endif
};

template<class MPITrans, class NumaTrans, class Type>
using FullDistDefault = FullDist<MPITrans, NumaTrans, Type, allocator::Default>;

template<class MPITrans, class NumaTrans, class Type>
using FullDistMPI = FullDist<MPITrans, NumaTrans, Type, allocator::MPIAlloc>;

}

}

#endif // GRID_LEVEL_FULLDIST_H

