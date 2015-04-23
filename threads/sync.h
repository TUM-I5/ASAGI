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
 * @copyright 2013-2014 Manuel Fasching <manuel.fasching@tum.de>
 * @copyright 2015 Sebastian Rettenberger <rettenbs@in.tum.de>
 */

#ifndef THREADS_SYNC_H
#define THREADS_SYNC_H

#include <pthread.h>

namespace threads
{

/**
 * @brief Synchronizes a set of threads
 *
 * {@link startBarrier()} and {@link endBarrier()} act as a barrier together.
 * In addition, any code in between those function calls is only executed by
 * one thread at a time.
 */
class Sync
{
private:
	/** Number of threads that already synchronized */
	unsigned int m_syncedThreads;

	/** Mutex used to synchronize the threads */
	pthread_mutex_t m_mutex;

	/** Condition variable to wait for all threads */
	pthread_cond_t m_condition;

	/** Number of threads that should finish last */
	unsigned int m_lastThreads;

	/** Condition variable for waiting "last threads" */
	pthread_cond_t m_lastCondition;

	/** True if we are in the finishing phase */
	bool m_finishing;

	/** Condition variable for threads waiting for the previous finishing phase */
	pthread_cond_t m_finishCondition;

	/** Pointer to the broadcasted data */
	void* m_data;

public:
	Sync()
		: m_syncedThreads(0),
		  m_mutex(PTHREAD_MUTEX_INITIALIZER),
		  m_condition(PTHREAD_COND_INITIALIZER),
		  m_lastThreads(0),
		  m_lastCondition(PTHREAD_COND_INITIALIZER),
		  m_finishing(false),
		  m_finishCondition(PTHREAD_COND_INITIALIZER),
		  m_data(0L)
	{
	}

	virtual ~Sync()
	{
		pthread_cond_destroy(&m_condition);
		pthread_mutex_destroy(&m_mutex);
		pthread_cond_destroy(&m_lastCondition);
		pthread_cond_destroy(&m_finishCondition);
	}

	/**
	 * Starts a barrier synchronization
	 *
	 * @return True on success, false otherwise
	 */
	bool startBarrier()
	{
		if (!pthread_mutex_lock(&m_mutex))

		if (m_finishing) {
			if (pthread_cond_wait(&m_finishCondition, &m_mutex))
				return false;
		}

		return true;
	}

	/**
	 * Waits in the barrier synchronization
	 *
	 * @param numThreads Number of threads that should wait in the barrier
	 * @param last All threads with the last flag will leave this function
	 *  after all threads without this flag
	 * @return True on success, false otherwise
	 */
	bool waitBarrier(unsigned int numThreads, bool last = false)
	{
		m_syncedThreads++;
		if (last)
			m_lastThreads++;

		if (m_syncedThreads == numThreads) {
			// We start the finishing phase now
			m_finishing = true;

			if (pthread_cond_broadcast(&m_condition) != 0) {
				pthread_mutex_unlock(&m_mutex);
				return false;
			}
		} else {
			if (pthread_cond_wait(&m_condition, &m_mutex) != 0) {
				pthread_mutex_unlock(&m_mutex);
				return false;
			}
		}

		// If this is a last, wait for all other threads
		if (last && m_syncedThreads > m_lastThreads) {
			if (pthread_cond_wait(&m_lastCondition, &m_mutex) != 0) {
				pthread_mutex_unlock(&m_mutex);
				return false;
			}
		}

		// The thread finished waiting
		m_syncedThreads--;
		if (last)
			m_lastThreads--;

		if (m_syncedThreads == m_lastThreads && !last) {
			// The last "not-last" thread can wake up all last threads
			if (pthread_cond_broadcast(&m_lastCondition) != 0) {
				pthread_mutex_unlock(&m_mutex);
				return false;
			}
		}

		if (m_syncedThreads == 0) {
			// Last synchronized threads
			m_finishing = false;
			pthread_cond_broadcast(&m_finishCondition);
		}

		return true;
	}

	/**
	 * Ends the barrier synchronization
	 *
	 * @return True on success, false otherwise
	 */
	bool endBarrier()
	{
		return pthread_mutex_unlock(&m_mutex) == 0;
	}

	/**
	 * Should be called between {@link startBarrier()} and {@link waitBarrier()}
	 *
	 * @return The number threads that are already waiting in {@link waitBarrier()}
	 */
	unsigned int waiting() const
	{
		return m_syncedThreads;
	}

	/**
	 * Same as calling {@link startBarrier()}, {@link waitBarrier()} and {@link endBarrier()}
	 */
	bool barrier(unsigned int numThreads)
	{
		if (!startBarrier())
			return false;
		if (!waitBarrier(numThreads))
			return false;
		return endBarrier();
	}

	/**
	 * Broadcast data to all threads
	 *
	 * @param data The data that should be broadcasted (has to be a pointer)
	 * @param numThreads Number of threads that should share the data
	 * @param current The thread id of the current thread
	 * @param root The thread with the original data
	 * @return True on success, false otherwise
	 */
	template<typename T>
	bool broadcast(T &data, unsigned int numThreads, unsigned int current, unsigned int root = 0)
	{
		if (!startBarrier())
			return false;

		if (current == root)
			m_data = &data;

		if (!waitBarrier(numThreads, current == root))
			return false;

		data = *static_cast<T*>(m_data);

		return endBarrier();
	}
};

}

#endif // THREADS_SYNC_H
