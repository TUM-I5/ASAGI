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

#ifndef NUMA_NUMA_H
#define NUMA_NUMA_H

#include <map>
#include <pthread.h>
#include <vector>

#include "asagi.h"

// Define the size of thread and numa id so we can store 2 ids
// in one pointer
#ifdef _LP64
typedef uint32_t id_t;
#else // _LP64
typedef id_t uint16_t;
#endif // _LP64

namespace numa
{

/**
 * Detects and handles NUMA domains
 */
class Numa
{
private:
	/** Total number of threads */
	unsigned int m_totalThreads;

	/** Maps from the NUMA domain to our domain identifier */
	std::map<int, unsigned int> m_domains;

	/** Temporary array to identify if the master thread already found for a domain */
	std::vector<bool> m_masterThreads;

	/** Number of threads that already synchronized */
	unsigned int m_syncedThreads;

	/** Mutex used to synchronize the threads */
	pthread_mutex_t m_mutex;

	/** Condition variable to wait for all threads */
	pthread_cond_t m_condition;

	/** The pthread key we use to store the domain identifier for each thread */
	pthread_key_t m_ptkey;

	/** True, if the key could not be created */
	bool m_keyError;
public:
	Numa();

	virtual ~Numa();

	/**
	 * Set the number of threads
	 */
	asagi::Grid::Error setThreads(unsigned int threads)
	{
		m_totalThreads = threads;
		return asagi::Grid::SUCCESS;
	}

	asagi::Grid::Error registerThread(bool &masterThread);

	/**
	 * @return The total number of threads
	 */
	unsigned int totalThreads() const
	{
		return m_totalThreads;
	}

	/**
	 * @return The total number of NUMA domains
	 */
	unsigned int totalDomains() const
	{
		return m_domains.size();
	}

	/**
	 * @return The id of this thread
	 */
	unsigned int threadId() const
	{
		uintptr_t ids = reinterpret_cast<std::uintptr_t>(pthread_getspecific(m_ptkey));
		return ids >> sizeof(id_t)*8;
	}

	/**
	 * @return The id of the NUMA domain for this thread
	 */
	unsigned int domainId() const
	{
		uintptr_t ids = reinterpret_cast<std::uintptr_t>(pthread_getspecific(m_ptkey));
		return ids & ((static_cast<uintptr_t>(1) << sizeof(id_t)*8) - 1);
	}
};

}

#endif // NUMA_NUMA_H
