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

#ifndef GRID_LEVEL_CACHE_H
#define GRID_LEVEL_CACHE_H

#include "blocked.h"
#include "allocator/default.h"
#include "cache/cachemanager.h"
#include "threads/mutex.h"

namespace grid
{

namespace level
{

/**
 * A grid that maintains a local cache for blocks. Can be combined with other
 * grid, e.g. StaticGrid.
 */
template<class MPIComm, class NumaComm, class Type, class Allocator = allocator::Default>
class Cache : public Blocked<Type>
{
private:
	/** Cache memory */
	unsigned char *m_cache;

	/** BlockManager used to control the cache */
	cache::CacheManager m_cacheManager;

	/** Mutex to lock the complete cache manager */
	threads::Mutex m_cacheMutex;

	/** Mutexes to lock the blocks in the cache */
	threads::Mutex *m_blockMutexes;

public:
	Cache(const mpi::MPIComm &comm,
			const numa::Numa &numa,
			Type &type)
		: Blocked<Type>(comm, numa, type),
		  m_cache(0L), m_blockMutexes(0L)
	{
	}

	virtual ~Cache()
	{
		Allocator::free(m_cache);
		delete [] m_blockMutexes;
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
		asagi::Grid::Error err = Blocked<Type>::open(filename, varname,
				blockSize, timeDimension, valuePos);
		if (err != asagi::Grid::SUCCESS)
			return err;

		// Allocate the memory
		err = Allocator::allocate(
				this->type().size_static() * this->totalBlockSize() * cacheSize,
				m_cache);
		if (err != asagi::Grid::SUCCESS)
			return err;

		// Initialize the cache manager
		m_cacheManager.init(cacheSize, cacheHandSpread);

		// Allocate the mutexes for the blocks
		m_blockMutexes = new threads::Mutex[cacheSize];

		return asagi::Grid::SUCCESS;
	}

	template<typename T>
	void getAt(T* buf, const double* pos)
	{
		this->incCounter(perf::Counter::ACCESS);

		// Get the index from the position
		size_t index[MAX_DIMENSIONS];
		this->pos2index(pos, index);

		// Get block id from the index
		unsigned long globalBlockId = this->blockByCoords(index);

		unsigned long cachePos;

		m_cacheMutex.lock();

		if (m_cacheManager.getIndex(globalBlockId, cachePos)) {
			// Block is in the cache
			// Lock the cache position but unlock the cache manager
			m_blockMutexes[cachePos].lock();
			m_cacheMutex.unlock();
		} else {
			// Block not in cache
			// Get a free block position
			long oldGlobalBlockId = m_cacheManager.getFreeIndex(globalBlockId, cachePos);

			// Lock the cache position and unlock the cache manager
			m_blockMutexes[cachePos].lock();
			m_cacheMutex.unlock();

			// Load the block
			loadBlock(index, cachePos, oldGlobalBlockId);
		}

		// Compute the offset in the block
		unsigned long offset = this->calcOffsetInBlock(index);

		// Finally, we fill the buffer
		this->type().convert(
				&m_cache[this->type().size_static() *
						(cachePos * this->totalBlockSize() + offset)],
				buf);

		// Free the block in the cache
		m_blockMutexes[cachePos].unlock();
	}

	/**
	 * Load a block from the file
	 *
	 * @param index The coordinates of the value
	 * @param cachePos The position in the cache very the block should be stored
	 * @param oldBlockId The block id that gets overridden or -1
	 */
	void loadBlock(const size_t *index, unsigned long cachePos, long oldBlockId)
	{
		this->incCounter(perf::Counter::FILE);

		size_t index2[MAX_DIMENSIONS];
		memcpy(index2, index, this->dimensions() * sizeof(size_t));

		// Get coordinates of the first value in the block
		for (unsigned char i = 0; i < this->dimensions(); i++)
			index2[i] -= index2[i] % this->blockSize(i);

		// Load the block
		this->type().load(this->inputFile(),
			index2, this->blockSize(),
			&m_cache[this->type().size_static() * this->totalBlockSize() * cachePos]);
	}

#if 0
	/**
	 * @return A pointer to the cached blocks
	 */
	unsigned char* getCache()
	{
		return m_cache;
	}
#endif
};

template<class MPIComm, class NumaComm, class Type>
using CacheDefault = Cache<MPIComm, NumaComm, Type>;

}

}

#endif /* GRID_LEVEL_CACHE_H */
