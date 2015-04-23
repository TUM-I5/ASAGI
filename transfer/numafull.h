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

#ifndef TRANSFER_NUMAFULL_H
#define TRANSFER_NUMAFULL_H

#include <cstring>

namespace transfer
{

/**
 * Copies blocks between NUMA assuming full storage
 */
class NumaFull
{
private:
	/** Pointer to the local static memory */
	const unsigned char* m_data;

	/** Number of blocks per NUMA domain */
	unsigned long m_blockCount;

	/** Block size (in bytes) */
	unsigned long m_blockSize;

	/** The rank of this process */
	int m_rank;

public:
	NumaFull()
		: m_data(0L),
		  m_blockCount(0), m_blockSize(0),
		  m_rank(-1)
	{
	}

	virtual ~NumaFull()
	{
	}

	/**
	 * Initialize the transfer class
	 */
	asagi::Grid::Error init(const unsigned char* data,
			unsigned long blockCount,
			unsigned long blockSize,
			const types::Type &type,
			const mpi::MPIComm &mpiComm,
			const numa::NumaComm &numaComm,
			cache::CacheManager &cacheManager)
	{
		m_blockCount = blockCount;
		m_blockSize = blockSize * type.size();

		// Compute the start of the memory
		m_data = &data[- static_cast<long>(numaComm.domainId()
				* m_blockSize * m_blockCount)];

		m_rank = mpiComm.rank();

		return asagi::Grid::SUCCESS;
	}

	/**
	 * Gets a block from the static storage of another NUMA domain
	 *
	 * @param blockId The global block id
	 * @param remoteRank The rank where the block is stored
	 * @param domainId Id of the NUMA domain that stores the data
	 * @param offset Offset of the block on the NUMA domain
	 * @param cache Pointer to the local cache for this block
	 */
	bool transfer(unsigned long blockId,
			int remoteRank, unsigned int domainId, unsigned long offset,
			unsigned char* cache)
	{
		if (remoteRank != m_rank)
			return false;

		memcpy(cache, &m_data[(m_blockCount * domainId + offset) * m_blockSize],
				m_blockSize);

		return true;
	}
};

}

#endif // TRANSFER_NUMAFULL_H

