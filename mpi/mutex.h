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
	/** The communicator we use to create the window and for send/recv */
	const MPIComm* m_comm;
	
	/** Rank were the lock array {@link m_lock} is stored */
	int m_homeRank;
	
	/**
	 * The lock array contains all the locks, only available
	 * on rank {@link m_homeRank} */
	long* m_lock;
	
	/** Local copy of the lock array */
	long* m_lockCopy;

	/** The window is used to store the locks */
	MPI_Win m_window;
	
	/**
	 * Allows to access all elements from {@link m_lock} without the
	 * current rank
	 */
	MPI_Datatype m_otherRanksType;
	
	/** The tag we use for send/recv */
	int m_tag;

	/**
	 * Make sure that {@link acquire} and {@link release} are only called
	 * by one thread
	 */
	threads::Mutex m_threadMutex;
public:
	Mutex();
	virtual ~Mutex();
	
	asagi::Grid::Error init(const MPIComm& comm);
	
	/**
	 * @brief Acquire a lock for a block
	 *
	 * @param block The block that should be locked
	 */
	void acquire(unsigned long block)
	{
		std::lock_guard<threads::Mutex> lock(m_threadMutex);

		int mpiResult; NDBG_UNUSED(mpiResult);

		// add self to lock list and get all other locks
		mpiResult = MPI_Win_lock(MPI_LOCK_EXCLUSIVE, m_homeRank,
			0, m_window);
		assert(mpiResult == MPI_SUCCESS);

		long myblock = block;
		mpiResult = MPI_Put(&myblock,
			1, MPI_LONG,
			m_homeRank, m_comm->rank(),
			1 , MPI_LONG,
			m_window);
		assert(mpiResult == MPI_SUCCESS);

		mpiResult = MPI_Get(m_lockCopy,
			m_comm->size() - 1, MPI_LONG,
			m_homeRank, 0,
			1, m_otherRanksType,
			m_window);
		assert(mpiResult == MPI_SUCCESS);

		mpiResult = MPI_Win_unlock(m_homeRank, m_window);

		// check to see if lock is already held
		int i;
		for (i = 0; i < (m_comm->size() - 1); i ++) {
			if (m_lockCopy[i] == static_cast<long>(block))
				break;
		}

		if (i < (m_comm->size() - 1)) {
			// wait for notification from some other process
			mpiResult = MPI_Recv(0L, 0, MPI_BYTE,
				MPI_ANY_SOURCE , m_tag, m_comm->comm(),
				MPI_STATUS_IGNORE);
			assert(mpiResult == MPI_SUCCESS);
		}
	}
	
	/**
	 * @brief Release the lock for a block
	 *
	 * Releasing a block without acquiring the lock is erroneous
	 */
	void release(unsigned long block)
	{
		std::lock_guard<threads::Mutex> lock(m_threadMutex);

		int mpiResult; NDBG_UNUSED(mpiResult);

		// remove self from waitlist
		mpiResult = MPI_Win_lock(MPI_LOCK_EXCLUSIVE, m_homeRank,
			0, m_window);
		assert(mpiResult == MPI_SUCCESS);

		mpiResult = MPI_Get(m_lockCopy,
			m_comm->size() - 1, MPI_LONG,
			m_homeRank, 0,
			1, m_otherRanksType,
			m_window);
		assert(mpiResult == MPI_SUCCESS);

		long myblock = -1;
		mpiResult = MPI_Put(&myblock,
			1, MPI_LONG,
			m_homeRank, m_comm->rank(),
			1, MPI_LONG,
			m_window);
		assert(mpiResult == MPI_SUCCESS);

		mpiResult = MPI_Win_unlock(m_homeRank, m_window);
		assert(mpiResult == MPI_SUCCESS);

		// find the next rank waiting for the lock (use round robin)
		for (int i = m_comm->rank(); i < (m_comm->size() - 1); i++) {
			if (m_lockCopy[i] == static_cast<long>(block)) {
				// Rank is of by one
				mpiResult = MPI_Send(0L, 0, MPI_BYTE,
					i+1, m_tag, m_comm->comm());
				assert(mpiResult == MPI_SUCCESS);
				return;
			}
		}
		for (int i = 0; i < m_comm->rank(); i++) {
			if (m_lockCopy[i] == static_cast<long>(block)) {
				mpiResult = MPI_Send(0L, 0, MPI_BYTE,
					i, m_tag, m_comm->comm());
				assert(mpiResult == MPI_SUCCESS);
				return;
			}
		}
	}

private:
	static int nextTag;

	static threads::Mutex nextTagMutex;
};

}

#endif // MPI_MUTEX_H
