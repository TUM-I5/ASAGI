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

#ifndef THREADS_PTHREADMUTEX_H
#define THREADS_PTHREADMUTEX_H

#ifndef USE_PTHREAD
#error "PThreads are not enabled."
#endif // USE_PTHREAD

#include <pthread.h>

namespace threads
{

/**
 * Mutex based on pthread spin locks
 */
class PthreadMutex
{
private:
	/** The pthread spin lock */
#ifndef __APPLE__
        pthread_spinlock_t m_lock;
#else
        pthread_mutex_t m_lock;
#endif // _APPLE__

public:
#ifndef __APPLE__
	PthreadMutex()
	{
		pthread_spin_init(&m_lock, PTHREAD_PROCESS_PRIVATE);
	}

	~PthreadMutex()
	{
		pthread_spin_destroy(&m_lock);
	}

	/**
	 * Lock the mutex
	 */
	void lock()
	{
		pthread_spin_lock(&m_lock);
	}

	/**
	 * Try to lock the mutex
	 */
	bool try_lock()
	{
		return pthread_spin_trylock(&m_lock) == 0;
	}

	/**
	 * Unlock the mutex
	 */
	void unlock()
	{
		pthread_spin_unlock(&m_lock);
	}
#else
        PthreadMutex()
        {
            pthread_mutexattr_t attr;
            pthread_mutexattr_init(&attr);
            pthread_mutexattr_settype(&attr, PTHREAD_PROCESS_PRIVATE);
            pthread_mutex_init(&m_lock, &attr);
        }

        ~PthreadMutex()
        {
            pthread_mutex_destroy(&m_lock);
        }

        /**
         * Lock the mutex
         */
        void lock()
        {
            pthread_mutex_lock(&m_lock);
        }

        /**
         * Try to lock the mutex
         */
        bool try_lock()
        {
            return pthread_mutex_trylock(&m_lock) == 0;
        }

        /**
         * Unlock the mutex
         */
        void unlock()
        {
            pthread_mutex_unlock(&m_lock);
        }
#endif // __APPLE__
};

}

#endif // THREADS_PTHREADMUTEX_H
