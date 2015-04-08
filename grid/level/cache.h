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
	/** Manager used to control the cache */
	cache::CacheManager<Allocator> m_cacheManager;

public:
	Cache()
		: Blocked<Type>()
	{
	}

	Cache(const mpi::MPIComm &comm,
			const numa::Numa &numa,
			Type &type)
		: Blocked<Type>(comm, numa, type)
	{
	}

	virtual ~Cache()
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
		asagi::Grid::Error err = Blocked<Type>::open(filename, varname,
				blockSize, timeDimension, valuePos);
		if (err != asagi::Grid::SUCCESS)
			return err;

		// Initialize the cache manager
		err = m_cacheManager.init(cacheSize,
				this->typeSize() * this->totalBlockSize(),
				cacheHandSpread);
		if (err != asagi::Grid::SUCCESS)
			return err;

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

		unsigned long cacheId;
		unsigned char* data;
		long oldGlobalBlockId = m_cacheManager.get(globalBlockId, cacheId, data);

		if (static_cast<long>(globalBlockId) != oldGlobalBlockId)
			// Cache not filled, do this first
			loadBlock(index, data);

		// Compute the offset in the block
		unsigned long offset = this->calcOffsetInBlock(index);

		assert(offset < this->totalBlockSize());

		// Finally, we fill the buffer
		this->type().convert(&data[this->typeSize() * offset], buf);

		// Free the block in the cache
		m_cacheManager.unlock(cacheId);
	}

	/**
	 * Load a block from the file
	 *
	 * @param index The coordinates of the value
	 * @param data Memory where the block should be stored
	 * @param cachePos The position in the cache very the block should be stored
	 */
	void loadBlock(const size_t *index, unsigned char* data)
	{
		this->incCounter(perf::Counter::FILE);

		size_t index2[MAX_DIMENSIONS];

		// Get coordinates of the first value in the block
		for (unsigned char i = 0; i < this->dimensions(); i++)
			index2[i] = index[i] - (index[i] % this->blockSize(i));

		// Load the block
		this->type().load(this->inputFile(),
			index2, this->blockSize(),
			data);
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
