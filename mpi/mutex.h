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

#ifndef MPI_MUTEX_H
#define MPI_MUTEX_H

#include "asagi.h"

#include <cassert>
#include <mutex>

#include "utils/logger.h"

#include "mpicomm.h"
#include "threads/mutex.h"

/**
 * @brief Extends the functionality of MPI
 */
namespace mpi {

/**
 * See "Implementing Byte-Range Locks Using MPI One-Sided Communication" for
 * details about this algorithm.
 * 
 * This class only works with at least 2 MPI processes.
 *
 * This class will <b>NOT</b> lock MPIComm::mpiMutex!
 */
class Mutex
{
private:
	/** The communicator associated with the window */
	const MPIComm* m_comm;

	/** The window is used to store the locks */
	MPI_Win m_window;
	
	/** The numa communicator */
	const numa::NumaComm* m_numa;

	/** The tag offset we use for send/recv */
	int m_tagOffset;

public:
	Mutex()
		: m_comm(0L),
		  m_window(MPI_WIN_NULL),
		  m_numa(0L),
		  m_tagOffset(0)
	{
	}

	virtual ~Mutex()
	{
	}

	/**
	 * Initialize the mutex
	 *
	 * @param comm The communicator used for this mutex.
	 * @param window The MPI window where the mutex values are stored
	 * @param numThreads The number of threads per processor
	 */
	void init(MPIComm& comm, MPI_Win window, const numa::NumaComm& numa)
	{
		m_comm = &comm;
		m_window = window;
		m_numa = &numa;

		m_tagOffset = comm.reserveTags(numa.totalThreads());
	}
	
	/**
	 * Initialize a mutex memory location
	 *
	 * @param lock Reference to the mutex memory location
	 */
	void initMutexMem(long& lock)
	{
		lock = MUTEX_UNLOCKED;
	}

	/**
	 * @brief Acquire a lock
	 *
	 * The caller has to make sure that the window is locked exclusively (e.g. with
	 * MPI_Win_lock) before/after the acquire call. This function will call
	 * MPI_Win_flush_local on the MPI window.
	 *
	 * @param rank The rank where the lock is stored
	 * @param offset The offset in the window (in window units) where the local is
	 *  stored
	 * @return A value < 0 if the mutex could be acquired. If a value >= 0 is
	 *  returned the caller has to call {@link wait(long)} afterwards to finish the
	 *  acquisition. In this case the mutex was locked.
	 */
	long acquire(int rank, unsigned long offset)
	{
		int mpiResult; NDBG_UNUSED(mpiResult);

		long newLock = lockId(m_numa->threadId());
		long oldLock;
		mpiResult = MPI_Fetch_and_op(&newLock, &oldLock, MPI_LONG, rank,
				offset, MPI_REPLACE, m_window);
		assert(mpiResult == MPI_SUCCESS);

		mpiResult = MPI_Win_flush_local(rank, m_window);
		assert(mpiResult == MPI_SUCCESS);

		return oldLock;
	}

	/**
	 *
	 * @param oldLock
	 */
	void wait(long oldLock)
	{
		// Not waiting required
		if (oldLock < 0)
			return;

		int mpiResult; NDBG_UNUSED(mpiResult);

		// The rank and thread id of the remote process/thread
		// that holds the lock
		int rank = oldLock / m_numa->totalThreads();
		int tag = oldLock % m_numa->totalThreads();
		MPI_Ssend(0L, 0, MPI_BYTE, rank, tag, m_comm->comm());
		assert(mpiResult == MPI_SUCCESS);
	}
	
	/**
	 * @brief Release the lock for a block
	 *
	 * Releasing a block without acquiring the lock is erroneous.<br>
	 * The caller has to make sure that the window is locked exclusively (e.g. with
	 * MPI_Win_lock) before/after the acquire call. This function will call
	 * MPI_Win_flush_local on the MPI window.
	 *
	 * @param rank The rank where the lock is stored
	 * @param offset The offset in the window (in window units) where the local is
	 *  stored
	 */
	void release(int rank, unsigned long offset)
	{
		int mpiResult; NDBG_UNUSED(mpiResult);

		unsigned int threadId = m_numa->threadId();

		long mutexUnlocked = MUTEX_UNLOCKED;
		long myLock = lockId(threadId);
		long oldLock;
		mpiResult = MPI_Compare_and_swap(&mutexUnlocked, &myLock, &oldLock,
				MPI_LONG, rank, offset, m_window);
		assert(mpiResult == MPI_SUCCESS);

		mpiResult = MPI_Win_flush_local(rank, m_window);
		assert(mpiResult == MPI_SUCCESS);

		if (oldLock != myLock)
			mpiResult = MPI_Recv(0L, 0, MPI_BYTE, MPI_ANY_SOURCE, threadId,
					m_comm->comm(), MPI_STATUS_IGNORE);
			assert(mpiResult == MPI_SUCCESS);
	}

private:
	/**
	 * Computes the lock id used for this thread
	 */
	long lockId(unsigned int threadId) const
	{
		return m_comm->rank() * m_numa->totalThreads() + threadId;
	}

private:
	static const long MUTEX_UNLOCKED = -1;
};

}

#endif // MPI_MUTEX_H
