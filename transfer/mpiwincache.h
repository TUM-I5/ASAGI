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

#ifndef TRANSFER_MPIWINCACHE_H
#define TRANSFER_MPIWINCACHE_H

#ifdef ASAGI_NOMPI
#include "mpino.h"
#else // ASAGI_NOMPI
#include "asagi.h"

#include <algorithm>
#include <cstdlib>

#include "mpicache.h"
#include "allocator/mpialloc.h"
#include "mpi/mpicomm.h"
#include "mpi/mutex.h"
#include "mpi/lockassert.h"
#include "threads/mutex.h"
#endif // ASAGI_NOMPI

namespace transfer
{

#ifdef ASAGI_NOMPI
/** No MPI transfers with MPI */
typedef MPINo MPIWinCache;
#else // ASAGI_NOMPI

/**
 * Copies blocks between MPI processes assuming cache storage
 * using MPI windows
 */
class MPIWinCache : private MPICache<allocator::MPIAlloc, true>
{
private:
	/** The NUMA domain ID */
	unsigned int m_numaDomainId;

	/** Pointer to the cache memory of the node */
	unsigned char *m_cache;

	/** The MPI mutex to lock the blocks */
	mpi::Mutex* m_mpiMutex;

	/** The MPI window for the dictionary */
	MPI_Win m_dictWin;

	/** The MPI window used to access the cache */
	MPI_Win m_cacheWin;

	/** Mutex for the dictionary window */
	threads::Mutex *m_dictWinMutex;

	/** Mutex for the cache window */
	threads::Mutex *m_cacheWinMutex;

	/** Number of elements in one block */
	unsigned long m_blockSize;

	/** Size of an element */
	unsigned int m_typeSize;

	/** The type MPI type of an element */
	MPI_Datatype m_mpiType;


public:
	MPIWinCache()
		: m_numaDomainId(0),
		  m_cache(0L),
		  m_mpiMutex(0L),
		  m_dictWin(MPI_WIN_NULL),
		  m_cacheWin(MPI_WIN_NULL),
		  m_dictWinMutex(0L),
		  m_cacheWinMutex(0L),
		  m_blockSize(0),
		  m_typeSize(0),
		  m_mpiType(MPI_DATATYPE_NULL)
	{
	}

	virtual ~MPIWinCache()
	{
		if (m_numaDomainId == 0) {
			mpi::MPIComm::mpiLock.lock();
			delete m_mpiMutex;
			delete m_dictWinMutex;
			delete m_cacheWinMutex;

			if (m_dictWin != MPI_WIN_NULL)
				MPI_Win_free(&m_dictWin);
			if (m_cacheWin != MPI_WIN_NULL)
				MPI_Win_free(&m_cacheWin);
			mpi::MPIComm::mpiLock.unlock();
		}
	}

	/**
	 * Initialize the transfer class
	 *
	 * @param cache Pointer to the cache
	 * @param cacheSize Number of blocks in the cache
	 * @param cacheManager The cache manager
	 * @param blockCount Number local blocks
	 * @param blockSize Number of elements in one block
	 * @param type The data type of the elements
	 * @param mpiComm The MPI communicator
	 * @param numaComm The NUMA communicator
	 */
	asagi::Grid::Error init(unsigned char* cache,
			unsigned int cacheSize,
			const cache::CacheManager &cacheManager,
			unsigned long blockCount,
			unsigned long blockSize,
			const types::Type &type,
			mpi::MPIComm &mpiComm,
			numa::NumaComm &numaComm)
	{
		m_numaDomainId = numaComm.domainId();
		m_blockSize = blockSize;
		m_typeSize = type.size();
		m_mpiType = type.getMPIType();

		// Broadcast the node global cache pointer
		m_cache = cache;
		asagi::Grid::Error err = numaComm.broadcast(m_cache);
		if (err != asagi::Grid::SUCCESS)
			return err;

		// Setup the parent
		MPICache<allocator::MPIAlloc, true>::init(cacheSize,
				blockCount, mpiComm, numaComm);

		if (m_numaDomainId == 0) {
			std::lock_guard<mpi::Lock> lock(mpi::MPIComm::mpiLock);

			m_dictWinMutex = new threads::Mutex();

			if (MPI_Win_create(dictionary(),
				sizeof(long) * totalDictEntrySize() * blockCount * numaComm.totalDomains(),
				sizeof(long),
				MPI_INFO_NULL,
				mpiComm.comm(),
				&m_dictWin) != MPI_SUCCESS)
				return asagi::Grid::MPI_ERROR;

			// Create the MPI mutex
			m_mpiMutex = new mpi::Mutex();
			m_mpiMutex->init(mpiComm, m_dictWin, numaComm);

			// Create cache window mutex
			m_cacheWinMutex = new threads::Mutex();

			// Create the window to access the cache
			if (MPI_Win_create(cache,
				m_typeSize * blockSize * rankCacheSize(),
				m_typeSize,
				MPI_INFO_NULL,
				mpiComm.comm(),
				&m_cacheWin) != MPI_SUCCESS)
				return asagi::Grid::MPI_ERROR;
		}

		err = numaComm.broadcast(m_dictWinMutex);
		if (err != asagi::Grid::SUCCESS)
			return err;

		err = numaComm.broadcast(m_dictWin);
		if (err != asagi::Grid::SUCCESS)
			return err;

		err = numaComm.broadcast(m_mpiMutex);
		if (err != asagi::Grid::SUCCESS)
			return err;

		err = numaComm.broadcast(m_cacheWinMutex);
		if (err != asagi::Grid::SUCCESS)
			return err;

		err = numaComm.broadcast(m_cacheWin);
		if (err != asagi::Grid::SUCCESS)
			return err;

		// Initialize the mutex memory
		for (unsigned long i = blockCount * m_numaDomainId;
				i < blockCount * (m_numaDomainId+1); i++)
			m_mpiMutex->initMutexMem(dictionary(i)[dictEntrySize()]);

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

		// Update the directory and get a potential rank that stores the data

#ifndef THREADSAFE_MPI
		// Lock the window to make sure no other thread interferes between
		// MPI_Win_lock and MPI_Win_unlock
		// Not required if we need to make MPI calls thread safe anyway
		// Include acquire() to avoid potential deadlocks
		m_dictWinMutex->lock();
#endif // THREADSAFE_MPI

		int mpiResult; NDBG_UNUSED(mpiResult);

		long dictEntry[MAX_DICT_SIZE];

		mpiResult = MPI_Win_lock(MPI_LOCK_EXCLUSIVE, dictRank, 0, m_dictWin);
		assert(mpiResult == MPI_SUCCESS);

		if (dictRank != mpiComm().rank()) {
			mpiResult = MPI_Get(dictEntry,
					dictEntrySize(),
					MPI_LONG,
					dictRank,
					dictOffset * totalDictEntrySize(),
					dictEntrySize(),
					MPI_LONG,
					m_dictWin);
			assert(mpiResult == MPI_SUCCESS);
		}

		long lock = m_mpiMutex->acquire(dictRank,
				dictEntrySize() + dictOffset * totalDictEntrySize());

		if (lock >= 0) {
			// We do not have the lock, wait re-read the dictionary entry
			mpiResult = MPI_Win_unlock(dictRank, m_dictWin);
			assert(mpiResult == MPI_SUCCESS);

#ifndef THREADSAFE_MPI
			m_dictWinMutex->unlock();
#endif // THREADSAFE_MPI

			m_mpiMutex->wait(lock);

#ifndef THREADSAFE_MPI
			m_dictWinMutex->lock();
#endif // THREADSAFE_MPI

			mpiResult = MPI_Win_lock(MPI_LOCK_EXCLUSIVE, dictRank, 0, m_dictWin);
			assert(mpiResult == MPI_SUCCESS);

			if (dictRank != mpiComm().rank()) {
				mpiResult = MPI_Get(dictEntry,
						dictEntrySize(),
						MPI_LONG,
						dictRank,
						dictOffset * totalDictEntrySize(),
						dictEntrySize(),
						MPI_LONG,
						m_dictWin);
				assert(mpiResult == MPI_SUCCESS);

				mpiResult = MPI_Win_flush_local(dictRank, m_dictWin);
				assert(mpiResult == MPI_SUCCESS);
			}
		}

		// Update the entry
		long entry;

		if (dictRank == mpiComm().rank())
			entry = fetchAndUpdateBlockInfo(dictionary(dictOffset),
					mpiComm().rank() * rankCacheSize() + offset);
		else {
			entry = fetchAndUpdateBlockInfo(dictEntry,
					mpiComm().rank() * rankCacheSize() + offset);

			mpiResult = MPI_Put(dictEntry,
					dictEntrySize(),
					MPI_LONG,
					dictRank,
					dictOffset * totalDictEntrySize(),
					dictEntrySize(),
					MPI_LONG,
					m_dictWin);
			assert(mpiResult == MPI_SUCCESS);
		}

		mpiResult = MPI_Win_unlock(dictRank, m_dictWin);
		assert(mpiResult == MPI_SUCCESS);

#ifndef THREADSAFE_MPI
		m_dictWinMutex->unlock();
#endif // THREADSAFE_MPI

		return entry;
	}

	/**
	 * Tries to transfers a block via MPI
	 *
	 * @param entry The dictionary entry obtained from {@link startTransfer}
	 * @param blockId The global id of the block
	 * @param cache Pointer to the local cache for this block
	 * @param[out] retry True of the transfer fails but MPI should be checked again
	 *  (always false)
	 * @return True if the block was fetched, false otherwise
	 */
	bool transfer(long entry, unsigned long blockId, unsigned char *cache, bool &retry)
	{
		retry = false;

		if (entry < 0)
			return false;

		int rank = entry / rankCacheSize();
		unsigned long offset = entry % rankCacheSize();

		assert(rank >= 0);

		if (rank == mpiComm().rank()) {
			// This might happen with MPI windows when using multiple threads
			// due to the tryLock() of the cache manager
			memcpy(cache, &m_cache[offset * m_blockSize * m_typeSize],
					m_blockSize * m_typeSize);
		} else {
			int mpiResult; NDBG_UNUSED(mpiResult);

#ifndef THREADSAFE_MPI
			// Lock the window to make sure no other thread interferes between
			// MPI_Win_lock and MPI_Win_unlock
			// Not required if we need to make MPI calls thread safe anyway
			std::lock_guard<threads::Mutex> winLock(*m_cacheWinMutex);
#endif // THREADSAFE_MPI

			// Lock remote window
			mpiResult = MPI_Win_lock(MPI_LOCK_SHARED, rank,
					ASAGI_MPI_MODE_NOCHECK, m_cacheWin);
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
		}

		return true;
	}

	/**
	 * Ends a transfer phase started with {@link startTransfer}
	 *
	 * @param blockId The ID of the block that was transfered
	 * @param dictRank The rank where the dictionary entry is stored
	 * @param dictOffset The offset of the dictionary entry on the rank
	 * @param offset The offset on the local rank where the block should be stored
	 */
	void endTransfer(unsigned long blockId, int dictRank,
			unsigned long dictOffset, unsigned long offset)
	{
#ifndef THREADSAFE_MPI
		// Lock the window to make sure no other thread interferes between
		// MPI_Win_lock and MPI_Win_unlock
		// Not required if we need to make MPI calls thread safe anyway
		// Include acquire() to avoid potential deadlocks
		std::lock_guard<threads::Mutex> winLock(*m_dictWinMutex);
#endif // THREADSAFE_MPI

		int mpiResult; NDBG_UNUSED(mpiResult);

		mpiResult = MPI_Win_lock(MPI_LOCK_EXCLUSIVE, dictRank, 0, m_dictWin);
		assert(mpiResult == MPI_SUCCESS);

		m_mpiMutex->release(dictRank,
				dictEntrySize() + dictOffset * totalDictEntrySize());

		mpiResult = MPI_Win_unlock(dictRank, m_dictWin);
		assert(mpiResult == MPI_SUCCESS);

		mpi::MPIComm::mpiLock.unlock();
	}

	/**
	 * Adds an entry to the dictionary
	 *
	 * @param blockId The ID of the block that should be transfered
	 * @param dictRank The rank where the dictionary entry is stored
	 * @param dictOffset The offset of the dictionary entry on the rank
	 * @param offset The offset on the local rank where the block should be stored
	 * @return A dictionary entry that has to be passed to {@link transfer} if
	 *  the block should be fetched from a remote rank
	 */
	void addBlock(unsigned long blockId, int dictRank,
			unsigned long dictOffset, unsigned long offset)
	{
		startTransfer(blockId, dictRank, dictOffset, offset);
		endTransfer(blockId, dictRank, dictOffset, offset);
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

		std::lock_guard<mpi::Lock> mpiLock(mpi::MPIComm::mpiLock);

		// Update the directory

#ifndef THREADSAFE_MPI
		// Lock the window to make sure no other thread interferes between
		// MPI_Win_lock and MPI_Win_unlock
		// Not required if we need to make MPI calls thread safe anyway
		// Include acquire() to avoid potential deadlocks
		m_dictWinMutex->lock();
#endif // THREADSAFE_MPI

		int mpiResult; NDBG_UNUSED(mpiResult);

		long dictEntry[MAX_DICT_SIZE];

		mpiResult = MPI_Win_lock(MPI_LOCK_EXCLUSIVE, dictRank, 0, m_dictWin);
		assert(mpiResult == MPI_SUCCESS);

		if (dictRank != mpiComm().rank()) {
			mpiResult = MPI_Get(dictEntry,
					dictEntrySize(),
					MPI_LONG,
					dictRank,
					dictOffset * totalDictEntrySize(),
					dictEntrySize(),
					MPI_LONG,
					m_dictWin);
			assert(mpiResult == MPI_SUCCESS);
		}

		long lock = m_mpiMutex->acquire(dictRank,
				dictEntrySize() + dictOffset * totalDictEntrySize());

		if (lock < 0) {
			// Just synchronize the window properly to make sure the release works
			mpiResult = MPI_Win_flush(dictRank, m_dictWin);
			assert(mpiResult == MPI_SUCCESS);
		} else {
			// We do not have the lock, wait re-read the dictionary entry
			mpiResult = MPI_Win_unlock(dictRank, m_dictWin);
			assert(mpiResult == MPI_SUCCESS);

#ifndef THREADSAFE_MPI
			m_dictWinMutex->unlock();
#endif // THREADSAFE_MPI

			m_mpiMutex->wait(lock);

#ifndef THREADSAFE_MPI
			m_dictWinMutex->lock();
#endif // THREADSAFE_MPI

			mpiResult = MPI_Win_lock(MPI_LOCK_EXCLUSIVE, dictRank, 0, m_dictWin);
			assert(mpiResult == MPI_SUCCESS);

			if (dictRank != mpiComm().rank()) {
				mpiResult = MPI_Get(dictEntry,
						dictEntrySize(),
						MPI_LONG,
						dictRank,
						dictOffset * totalDictEntrySize(),
						dictEntrySize(),
						MPI_LONG,
						m_dictWin);
				assert(mpiResult == MPI_SUCCESS);

				mpiResult = MPI_Win_flush_local(dictRank, m_dictWin);
				assert(mpiResult == MPI_SUCCESS);
			}
		}

		// Remove the entry
		if (dictRank == mpiComm().rank())
			deleteBlockInfo(dictionary(dictOffset),
					mpiComm().rank() * rankCacheSize() + offset);
		else {
			deleteBlockInfo(dictEntry,
					mpiComm().rank() * rankCacheSize() + offset);

			mpiResult = MPI_Put(dictEntry,
					dictEntrySize(),
					MPI_LONG,
					dictRank,
					dictOffset * totalDictEntrySize(),
					dictEntrySize(),
					MPI_LONG,
					m_dictWin);
			assert(mpiResult == MPI_SUCCESS);
		}

		m_mpiMutex->release(dictRank,
				dictEntrySize() + dictOffset * totalDictEntrySize());

		mpiResult = MPI_Win_unlock(dictRank, m_dictWin);
		assert(mpiResult == MPI_SUCCESS);

#ifndef THREADSAFE_MPI
		m_dictWinMutex->unlock();
#endif // THREADSAFE_MPI
	}
};

#endif // ASAGI_NOMPI

}

#endif // TRANSFER_MPIWINCACHE_H

