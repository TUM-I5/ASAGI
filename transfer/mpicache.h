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

#ifndef TRANSFER_MPICACHE_H
#define TRANSFER_MPICACHE_H

#ifdef ASAGI_NOMPI
#include "mpino.h"
#else // ASAGI_NOMPI
#include "asagi.h"

#include <algorithm>
#include <cstdlib>

#include "allocator/mpialloc.h"
#include "mpi/mpicomm.h"
#include "mpi/mutex.h"
#endif // ASAGI_NOMPI

namespace transfer
{

#ifdef ASAGI_NOMPI
/** No MPI transfers with MPI */
typedef MPINo MPICache;
#else // ASAGI_NOMPI

/**
 * Copies blocks between MPI processes assuming cache storage
 */
class MPICache
{
private:
	/** The MPI communicator */
	const mpi::MPIComm* m_mpiComm;

	/** The NUMA domain ID */
	unsigned int m_numaDomainId;

	/** The MPI mutex to lock the blocks */
	mpi::Mutex* m_mpiMutex;

	/** Number of dictionary entries */
	unsigned int m_dictEntrySize;

	/** Local part of the dictionary */
	long* m_dictionary;

	/** The MPI window for the dictionary */
	MPI_Win m_dictWin;

	/** The MPI window used to access the cache */
	MPI_Win m_cacheWin;

	/** Number of blocks on one rank */
	unsigned long m_rankCacheSize;

	/** Number of elements in one block */
	unsigned long m_blockSize;

	/** The type MPI type of an element */
	MPI_Datatype m_mpiType;


public:
	MPICache()
		: m_mpiComm(0L),
		  m_numaDomainId(0),
		  m_mpiMutex(0L),
		  m_dictEntrySize(4), // Default (can not be changed at the moment)
		  m_dictionary(0L),
		  m_dictWin(MPI_WIN_NULL),
		  m_cacheWin(MPI_WIN_NULL),
		  m_rankCacheSize(0),
		  m_blockSize(0),
		  m_mpiType(MPI_DATATYPE_NULL)
	{
	}

	virtual ~MPICache()
	{
		if (m_numaDomainId == 0) {
			mpi::MPIComm::mpiLock.lock();
			delete m_mpiMutex;

			if (m_dictWin != MPI_WIN_NULL)
				MPI_Win_free(&m_dictWin);
			if (m_cacheWin != MPI_WIN_NULL)
				MPI_Win_free(&m_cacheWin);
			mpi::MPIComm::mpiLock.unlock();

			allocator::MPIAlloc::free(m_dictionary);
		}
	}

	/**
	 * Initialize the transfer class
	 *
	 * @param cache Pointer to the cache
	 * @param cacheSize Number of blocks in the cache
	 * @param blockCount Number local blocks
	 * @param blockSize Number of elements in one block
	 * @param type The data type of the elements
	 * @param mpiComm The MPI communicator
	 * @param numaComm The NUMA communicator
	 */
	asagi::Grid::Error init(unsigned char* cache,
			unsigned int cacheSize,
			unsigned long blockCount,
			unsigned long blockSize,
			const types::Type &type,
			const mpi::MPIComm &mpiComm,
			numa::NumaComm &numaComm)
	{
		m_mpiComm = &mpiComm;
		m_numaDomainId = numaComm.domainId();
		m_blockSize = blockSize;
		m_mpiType = type.getMPIType();

		m_rankCacheSize = cacheSize * numaComm.totalDomains();

		if (m_numaDomainId == 0) {
			// Create the dictionary
			allocator::MPIAlloc::allocate(
					blockCount * m_dictEntrySize * numaComm.totalDomains(),
					m_dictionary);

			std::lock_guard<mpi::Lock> lock(mpi::MPIComm::mpiLock);

			if (MPI_Win_create(m_dictionary,
				sizeof(long) * m_dictEntrySize * blockCount,
				sizeof(long),
				MPI_INFO_NULL,
				mpiComm.comm(),
				&m_dictWin) != MPI_SUCCESS)
				return asagi::Grid::MPI_ERROR;

			// Create the MPI mutex
			m_mpiMutex = new mpi::Mutex();
			asagi::Grid::Error err = m_mpiMutex->init(mpiComm);
			if (err != asagi::Grid::SUCCESS)
				return err;

			// Create the window to access the cache
			unsigned int typeSize = type.size();

			if (MPI_Win_create(cache,
				typeSize * blockSize * m_rankCacheSize,
				typeSize,
				MPI_INFO_NULL,
				mpiComm.comm(),
				&m_cacheWin) != MPI_SUCCESS)
				return asagi::Grid::MPI_ERROR;
		}

		asagi::Grid::Error err = numaComm.broadcast(m_dictionary);
		if (err != asagi::Grid::SUCCESS)
			return err;

		for (unsigned long i = blockCount * m_dictEntrySize * m_numaDomainId;
				i < blockCount * m_dictEntrySize * (m_numaDomainId+1); i++)
			m_dictionary[i] = -1;

		err = numaComm.broadcast(m_dictWin);
		if (err != asagi::Grid::SUCCESS)
			return err;

		err = numaComm.broadcast(m_mpiMutex);
		if (err != asagi::Grid::SUCCESS)
			return err;

		err = numaComm.broadcast(m_cacheWin);
		if (err != asagi::Grid::SUCCESS)
			return err;

		return asagi::Grid::SUCCESS;
	}

	/**
	 * Starts the transfer of a new block.
	 *
	 * This will lock the global dictionary. Use {@link endTransfer}
	 * To end the transfer phase and free the dictionary block.
	 *
	 * @param blockId The ID of the block that should be transfered
	 * @param dictRank The rank where the dictionary entry is stored
	 * @param dictOffset The offset of the dictionary entry on the rank
	 * @param offset The offset on the local rank where the block should be stored
	 * @return A dictionary entry that has to be passed to {@link transfer} if
	 *  the block should be fetched from a remote rank
	 */
	long startTransfer(unsigned long blockId, int dictRank,
			unsigned long dictOffset, unsigned long offset)
	{
		// Only lock library once, to avoid deadlocks
		mpi::MPIComm::mpiLock.lock();

		m_mpiMutex->acquire(blockId);

		// Update the directory and get a potential rank that stores the data

		int mpiResult; NDBG_UNUSED(mpiResult);

		mpiResult = MPI_Win_lock(MPI_LOCK_SHARED, dictRank,
				MPI_MODE_NOCHECK, m_dictWin);
		assert(mpiResult == MPI_SUCCESS);

		long entry;

		if (dictRank == m_mpiComm->rank())
			entry = fetchAndUpdateBlockInfo(
					&m_dictionary[dictOffset * m_dictEntrySize],
					offset);
		else {
			// Local buffer for a dict entry
			long dictEntry[MAX_DICT_SIZE];

			mpiResult = MPI_Get(dictEntry,
					m_dictEntrySize,
					MPI_LONG,
					dictRank,
					dictOffset * m_dictEntrySize,
					m_dictEntrySize,
					MPI_LONG,
					m_dictWin);
			assert(mpiResult == MPI_SUCCESS);

			mpiResult = MPI_Win_unlock(dictRank, m_dictWin);
			assert(mpiResult == MPI_SUCCESS);

			entry = fetchAndUpdateBlockInfo(dictEntry,	offset);

			mpiResult = MPI_Win_lock(MPI_LOCK_SHARED, dictRank,
					MPI_MODE_NOCHECK, m_dictWin);
			assert(mpiResult == MPI_SUCCESS);

			mpiResult = MPI_Put(dictEntry,
					m_dictEntrySize,
					MPI_LONG,
					dictRank,
					dictOffset * m_dictEntrySize,
					m_dictEntrySize,
					MPI_LONG,
					m_dictWin);
			assert(mpiResult == MPI_SUCCESS);
		}

		mpiResult = MPI_Win_unlock(dictRank, m_dictWin);
		assert(mpiResult == MPI_SUCCESS);

		return entry;
	}

	/**
	 * Tries to transfers a block via MPI
	 *
	 * @param entry The dictionary entry obtained from {@link startTransfer}
	 * @param cache Pointer to the local cache for this block
	 * @return True if the block was fetched, false otherwise
	 */
	bool transfer(long entry, unsigned char *cache)
	{
		if (entry < 0)
			return false;

		int rank = entry / m_rankCacheSize;
		unsigned long offset = entry % m_rankCacheSize;

		assert(rank >= 0);

		int mpiResult; NDBG_UNUSED(mpiResult);

		// Lock remote window
		mpiResult = MPI_Win_lock(MPI_LOCK_SHARED, rank,
				MPI_MODE_NOCHECK, m_cacheWin);
		assert(mpiResult == MPI_SUCCESS);

		// Transfer data
		mpiResult = MPI_Get(cache,
				m_blockSize,
				m_mpiType,
				rank,
				offset * m_blockSize,
				m_blockSize,
				m_mpiType,
				m_cacheWin);
		assert(mpiResult == MPI_SUCCESS);

		// Unlock remote window
		mpiResult = MPI_Win_unlock(rank, m_cacheWin);
		assert(mpiResult == MPI_SUCCESS);

		return true;
	}

	/**
	 * Ends a transfer phase started with {@link startTransfer}
	 *
	 * @param blockId The ID of the block that was transfered
	 */
	void endTransfer(unsigned long blockId)
	{
		m_mpiMutex->release(blockId);

		mpi::MPIComm::mpiLock.unlock();
	}

	/**
	 * Deletes information about a local stored block
	 *
	 * @param blockId The global block id
	 * @param dictRank The rank where the dictionary entry is stored
	 * @param dictOffset The offset of the dictionary entry on the rank
	 * @param offset The offset on the local rank
	 */
	void deleteBlock(long blockId, int dictRank, unsigned long dictOffset,
			unsigned long offset)
	{
		if (blockId < 0)
			// Invalid block id
			return;

		int mpiResult; NDBG_UNUSED(mpiResult);

		std::lock_guard<mpi::Lock> lock(mpi::MPIComm::mpiLock);

		m_mpiMutex->acquire(blockId);

		mpiResult = MPI_Win_lock(MPI_LOCK_SHARED, dictRank,
			MPI_MODE_NOCHECK, m_dictWin);
		assert(mpiResult == MPI_SUCCESS);

		if (dictRank == m_mpiComm->rank())
			deleteBlockInfo(&m_dictionary[dictOffset * m_dictEntrySize],
					offset);
		else {
			// Local buffer for a dict entry
			long dictEntry[MAX_DICT_SIZE];

			mpiResult = MPI_Get(dictEntry,
				m_dictEntrySize,
				MPI_LONG,
				dictRank,
				dictOffset * m_dictEntrySize,
				m_dictEntrySize,
				MPI_LONG,
				m_dictWin);
			assert(mpiResult == MPI_SUCCESS);

			mpiResult = MPI_Win_unlock(dictRank, m_dictWin);
			assert(mpiResult == MPI_SUCCESS);

			deleteBlockInfo(dictEntry, offset);

			mpiResult = MPI_Win_lock(MPI_LOCK_SHARED, dictRank,
				MPI_MODE_NOCHECK, m_dictWin);
			assert(mpiResult == MPI_SUCCESS);

			mpiResult = MPI_Put(dictEntry,
				m_dictEntrySize,
				MPI_LONG,
				dictRank,
				dictOffset * m_dictEntrySize,
				m_dictEntrySize,
				MPI_LONG,
				m_dictWin);
			assert(mpiResult == MPI_SUCCESS);
		}

		mpiResult = MPI_Win_unlock(dictRank, m_dictWin);
		assert(mpiResult == MPI_SUCCESS);

		m_mpiMutex->release(blockId);
	}

private:
	/**
	 * Fetch and update block info
	 *
	 * @param dictEntry The dictionary entry
	 * @param offset The offset on this rank (where the block will be stored)
	 * @return A dictionary entry from where the block can be transfered
	 */
	long fetchAndUpdateBlockInfo(long* dictEntry, unsigned long offset)
	{
		unsigned int count;
		for (count = 0; count < m_dictEntrySize; count++)
			if (dictEntry[count] < 0)
				break;

		long entry = -1;
		if (count > 0)
			entry = dictEntry[rand() % count];

		count = std::min(count, m_dictEntrySize-1); // we don't need t move the last entry
													// if the array is full
		for (int i = count-1; i >= 0; i--)
			dictEntry[i+1] = dictEntry[i];

		// New entry
		dictEntry[0] = m_mpiComm->rank() * m_rankCacheSize + offset;

		return entry;
	}

	/**
	 * Remove the the current rank form the entry
	 *
	 * @param dictEntry The dictionary entry
	 * @param offset The offset on this rank
	 */
	void deleteBlockInfo(long* dictEntry, unsigned long offset)
	{
		long entry = m_mpiComm->rank() * m_rankCacheSize + offset;

		unsigned int i;
		for (i = 0; i < m_dictEntrySize; i++) {
			if (dictEntry[i] == entry)
				break;
		}

		for (; i < m_dictEntrySize-1; i++)
			dictEntry[i] = dictEntry[i+1];

		dictEntry[m_dictEntrySize-1] = -1;
	}

private:
	/** Maximum allowed dictionary size */
	const static unsigned int MAX_DICT_SIZE = 8;
};

#endif // ASAGI_NOMPI

}

#endif // TRANSFER_MPICACHE_H

