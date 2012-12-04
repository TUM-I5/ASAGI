/**
 * @file
 *  This file is part of ASAGI
 *
 *  ASAGI is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  ASAGI is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with ASAGI.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Diese Datei ist Teil von ASAGI.
 *
 *  ASAGI ist Freie Software: Sie koennen es unter den Bedingungen
 *  der GNU General Public License, wie von der Free Software Foundation,
 *  Version 3 der Lizenz oder (nach Ihrer Option) jeder spaeteren
 *  veroeffentlichten Version, weiterverbreiten und/oder modifizieren.
 *
 *  ASAGI wird in der Hoffnung, dass es nuetzlich sein wird, aber
 *  OHNE JEDE GEWAEHELEISTUNG, bereitgestellt; sogar ohne die implizite
 *  Gewaehrleistung der MARKTFAEHIGKEIT oder EIGNUNG FUER EINEN BESTIMMTEN
 *  ZWECK. Siehe die GNU General Public License fuer weitere Details.
 *
 *  Sie sollten eine Kopie der GNU General Public License zusammen mit diesem
 *  Programm erhalten haben. Wenn nicht, siehe <http://www.gnu.org/licenses/>.
 * 
 * @copyright 2012 Sebastian Rettenberger <rettenbs@in.tum.de>
 */

#ifndef GRID_LOCALCACHEGRID_H
#define GRID_LOCALCACHEGRID_H

#include "grid.h"

#include "blocks/blockmanager.h"

#ifndef THREADSAFETY
#include <mutex>
#endif // THREADSAFETY

namespace grid
{

class LocalCacheGrid : virtual public Grid
{
private:
	/** Cache memory */
	unsigned char *m_cache;

	/** BlockManager used to control the cache */
	blocks::BlockManager m_blockManager;

#ifdef THREADSAFETY
	/**
	 * Lock cache
	 * @todo Use a shared mutex, to allow multiple readers
	 */
	std::mutex m_slaveMutex;
#endif // THREADSAFETY

public:
	LocalCacheGrid(const GridContainer &container,
		unsigned int hint = asagi::Grid::NO_HINT);
	virtual ~LocalCacheGrid();

protected:
	virtual asagi::Grid::Error init();

	virtual void getAt(void* buf, types::Type::converter_t converter,
			unsigned long x, unsigned long y, unsigned long z);

	virtual void getBlock(unsigned long block,
		long oldBlock,
		unsigned long cacheIndex,
		unsigned char* cache);

	/**
	 * Allocate the memory that holds the local cache.
	 * Can be overwritten for special allocation
	 */
	virtual asagi::Grid::Error allocCache(size_t size, unsigned char *&cache)
	{
		cache = new unsigned char[size];
		return asagi::Grid::SUCCESS;
	}

	/**
	 * Frees local cache
	 *
	 * @see allocCache
	 */
	virtual void freeCache(unsigned char *cache)
	{
		delete [] cache;
	}

	/**
	 * @return A pointer to the cached blocks
	 */
	unsigned char* getCache()
	{
		return m_cache;
	}

	/**
	 * When overriding {@link getBlock}, you can also override this
	 * function to return false.
	 *
	 * @see Grid::keepFileOpen()
	 */
	virtual bool keepFileOpen() const
	{
		return true;
	}
};

}

#endif /* GRID_LOCALCACHEGRID_H */
