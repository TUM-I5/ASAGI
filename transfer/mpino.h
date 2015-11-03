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

#ifndef TRANSFER_MPINO_H
#define TRANSFER_MPINO_H

namespace transfer
{

/**
 * Disables MPI transfers
 */
class MPINo
{
public:
	MPINo()
	{
	}

	virtual ~MPINo()
	{
	}

	/**
	 * Initialize the transfer class
	 *
	 * Stub for {@link MPIWinFull::init} and {@link MPIThreadFull::init}
	 */
	asagi::Grid::Error init(unsigned char* data,
			unsigned long blockCount,
			unsigned long blockSize,
			const types::Type &type,
			const mpi::MPIComm &mpiComm,
			const numa::NumaComm &numaComm)
	{
		return asagi::Grid::SUCCESS;
	}

	/**
	 * Initialize the transfer class
	 *
	 * Stub for {@link MPICache::init}
	 */
	asagi::Grid::Error init(unsigned char* cache,
			unsigned int cacheSize,
			const cache::CacheManager &cachemManager,
			unsigned long blockCount,
			unsigned long blockSize,
			const types::Type &type,
			const mpi::MPIComm &mpiComm,
			const numa::NumaComm &numaComm)
	{
		return asagi::Grid::SUCCESS;
	}

	/**
	 * Should never be called
	 *
	 * Stub for {@link MPIWinFull::transfer} and {@link MPIThreadFull::transfer}
	 */
	void transfer(int remoteRank, unsigned long offset,
			unsigned char *cache)
	{
	}

	/**
	 * Starts the transfer of a new block.
	 *
	 * Stub for {@link MPIWinCache::startTransfer} and
	 * {@link MPIThreadCache::startTransfer}
	 */
	long startTransfer(unsigned long blockId, int dictRank,
			unsigned long dictOffset, unsigned long offset)
	{
		return -1;
	}

	/**
	 * Will not transfer anything
	 *
	 * Stub for {@link MPIWinCache::transfer} and
	 * {@link MPIThreadCache::transfer}
	 */
	bool transfer(long entry, unsigned long blockId, unsigned char *cache, bool &retry)
	{
		retry = false;
		return false;
	}

	/**
	 * Ends a transfer phase started with {@link startTransfer}
	 *
	 * Stub for {@link MPIWinCache::endTransfer} and
	 * {@link MPIThreadCache::endTransfer}
	 */
	void endTransfer(unsigned long blockId, int dictRank,
			unsigned long dictOffset, unsigned long offset)
	{
	}

	/**
	 * Adds information about a local stored block
	 *
	 * Stub for {@link MPIWinCache::addBlock} and
	 * {@link MPIThreadCache::addBlock}
	 */
	void addBlock(unsigned long blockId, int dictRank,
			unsigned long dictOffset, unsigned long offset)
	{
	}

	/**
	 * Deletes information about a local stored block
	 *
	 * Stub for {@link MPIWinCache::deleteBlock} and
	 * {@link MPIThreadCache::deleteBlock}
	 */
	void deleteBlock(long blockId, int dictRank, unsigned long dictOffset,
			unsigned long offset)
	{
	}
};

}

#endif // TRANSFER_MPINO_H

