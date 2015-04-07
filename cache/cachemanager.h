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

#ifndef CACHE_CHACHEMANAGER_H
#define CACHE_CHACHEMANAGER_H

#include <cstring>

#include "cachelist.h"
#include "threads/mutex.h"

namespace cache
{

/**
 * @brief Manages a block cache
 */
template<class Allocator>
class CacheManager
{
private:
	/** Cache memory */
	unsigned char *m_cache;

	/** The size of one block in the cache in bytes */
	unsigned long m_blockSize;

	/** Mutex to lock the complete cache manager */
	threads::Mutex m_cacheMutex;

	/** List of cached blocks */
	CacheList m_cacheList;

	/** Mutexes to lock the blocks in the cache */
	threads::Mutex *m_blockMutexes;

public:
	CacheManager()
		: m_cache(0L), m_blockSize(0), m_blockMutexes(0L)
	{
	}

	virtual ~CacheManager()
	{
		Allocator::free(m_cache);
		delete [] m_blockMutexes;
	}

	/**
	 * Initializes the cache manager
	 *
	 * @param blocks Number of blocks
	 * @param blockSize The size of one block in bytes
	 * @param handDiff Difference between the two hands in the
	 *  2-handed clock algorithm
	 */
	asagi::Grid::Error init(unsigned long blocks, unsigned long blockSize, long handDiff = -1)
	{
		m_blockSize = blockSize;

		m_cacheList.init(blocks, handDiff);

		// Allocate the mutexes for the blocks
		m_blockMutexes = new threads::Mutex[blocks];

		// Allocate the cache
		asagi::Grid::Error err = Allocator::allocate(blocks * blockSize, m_cache);
		if (err != asagi::Grid::SUCCESS)
			return err;

		// Initialize the memory to make sure it get allocated in the correct NUMA domain
		memset(m_cache, 0, blocks * blockSize);

		return asagi::Grid::SUCCESS;
	}

	/**
	 * Get the cache entry for the the block id.
	 * This function will lock the cache entry for further access. Use
	 * {@link unlock} to unlock the cache entry.
	 *
	 * @param blockId The requested block id
	 * @param[out] cacheId Identifier that can be passed to {@link unlock} to unlock
	 *  the cache entry
	 * @param[out] data A pointer to the request cache entry
	 * @return The block id of the element that is currently stored at the cache position.
	 *  If this is not equal to <code>blockId</code> the cache entry needs to be filled
	 *  first. -1 is returned if the cache entry needs to be filled but it is currently
	 *  empty/does not contain any valid block.
	 */
	long get(unsigned long blockId, unsigned long &cacheId, unsigned char* &data)
	{
		m_cacheMutex.lock();

		long oldBlockId = blockId;
		if (!m_cacheList.getIndex(blockId, cacheId))
			// Block not in cache
			// Get a free block position
			oldBlockId = m_cacheList.getFreeIndex(blockId, cacheId);

		// Lock the cache position and unlock the cache manager
		m_blockMutexes[cacheId].lock();
		m_cacheMutex.unlock();

		// Get the memory position
		data = &m_cache[cacheId * m_blockSize];

		return oldBlockId;
	}

	/**
	 * Unlock the cache entry previously locked with {@link get}
	 *
	 * @param cacheId The cache entry identifier
	 */
	void unlock(unsigned long cacheId)
	{
		m_blockMutexes[cacheId].unlock();
	}

};

}

#endif // CACHE_CACHEMANAGER_H
