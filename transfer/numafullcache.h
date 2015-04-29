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

#ifndef TRANSFER_NUMAFULLCACHE_H
#define TRANSFER_NUMAFULLCACHE_H

#include <cstring>

#include "numafull.h"
#include "numacache.h"
#include "cache/cachemanager.h"
#include "threads/mutex.h"

namespace transfer
{

/**
 * Copies blocks between NUMA assuming full storage.
 * Will also copy blocks from other caches.
 */
class NumaFullCache : public NumaFull, public NumaCache
{
public:
	NumaFullCache()
	{
	}

	virtual ~NumaFullCache()
	{
	}

	/**
	 * @copydoc NumaFull::init
	 */
	asagi::Grid::Error init(const unsigned char* data,
			unsigned long blockCount,
			unsigned long blockSize,
			const types::Type &type,
			const mpi::MPIComm &mpiComm,
			numa::NumaComm &numaComm,
			cache::CacheManager &cacheManager)
	{
		asagi::Grid::Error err = NumaFull::init(data, blockCount, blockSize,
				type, mpiComm, numaComm, cacheManager);
		if (err != asagi::Grid::SUCCESS)
			return err;

		err = NumaCache::init(blockSize, type, numaComm, cacheManager);
		if (err != asagi::Grid::SUCCESS)
			return err;

		return asagi::Grid::SUCCESS;
	}

	/**
	 * @copydoc NumaFill:transfer
	 * Will also lock into other NUMA caches
	 */
	bool transfer(unsigned long blockId,
			int remoteRank, unsigned int domainId, unsigned long offset,
			unsigned char* cache)
	{
		if (NumaFull::transfer(blockId, remoteRank, domainId, offset, cache))
			return true;

		return NumaCache::transfer(blockId, cache);
	}
};

}

#endif // TRANSFER_NUMAFULLCACHE_H

