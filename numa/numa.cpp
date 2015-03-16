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

#include "numa.h"

#include <numa.h>
#include <sched.h>

numa::Numa::Numa()
	: m_totalThreads(1), m_syncedThreads(0),
	  m_mutex(PTHREAD_MUTEX_INITIALIZER),
	  m_condition(PTHREAD_COND_INITIALIZER),
	  m_keyError(false)
{
	if (pthread_key_create(&m_ptkey, 0L) != 0)
		// remember this error
		m_keyError = true;
}

numa::Numa::~Numa()
{
	pthread_key_delete(m_ptkey);
	pthread_cond_destroy(&m_condition);
	pthread_mutex_destroy(&m_mutex);
}

/**
 * @brief Registers a thread and detects the NUMA domain for this thread.
 *
 * This is a collective function among all threads. It detects the domain id
 * for each thread and finds the master thread for each domain.
 * This function can be called multiple times by each thread to detect
 * the master thread more than once.
 *
 * @param masterThread True up on return, if this is the master thread,
 *  false otherwise
 */
asagi::Grid::Error numa::Numa::registerThread(bool &masterThread)
{
	// Get the NUMA domain
	int cpu = sched_getcpu();
	if (cpu < 0)
		return asagi::Grid::NUMA_ERROR;
	int domain = numa_node_of_cpu(cpu);
	if (domain < 0)
		return asagi::Grid::NUMA_ERROR;

	// Lock lock all variables
	if (pthread_mutex_lock(&m_mutex) != 0)
		return asagi::Grid::THREAD_ERROR;

	// Get thead id
	unsigned int threadId = m_syncedThreads;

	// Get the domain id from the NUMA domain
	unsigned int domainId;
	std::map<int, unsigned int>::const_iterator domIt
		= m_domains.find(domain);
	if (domIt == m_domains.end()) {
		// new domain
		domainId = m_domains.size();
		m_domains[domain] = domainId;
	} else
		domainId = domIt->second;

	// Detect if we are the master thread
	if (m_masterThreads.size() <= domainId)
		m_masterThreads.resize(domainId+1);
	masterThread = !m_masterThreads[domainId];
	m_masterThreads[domainId] = true;

	// This thread has finished
	m_syncedThreads++;

	if (m_syncedThreads == m_totalThreads) {
		// Cleanup for next iteration
		m_syncedThreads = 0;
		std::fill(m_masterThreads.begin(), m_masterThreads.end(), false);

		if (pthread_cond_broadcast(&m_condition) != 0) {
			pthread_mutex_unlock(&m_mutex);
			return asagi::Grid::THREAD_ERROR;
		}
	} else {
		if (pthread_cond_wait(&m_condition, &m_mutex) != 0) {
			pthread_mutex_unlock(&m_mutex);
			return asagi::Grid::THREAD_ERROR;
		}
	}

	// Synchronization done
	if (pthread_mutex_unlock(&m_mutex) != 0)
		return asagi::Grid::THREAD_ERROR;

	// Check if the key was created successfully
	if (m_keyError)
		return asagi::Grid::THREAD_ERROR;

	// Finally store the thread and domain id in the pthread key
	uintptr_t ids = (static_cast<uintptr_t>(threadId) << sizeof(id_t)*8) | domainId;

	if (pthread_setspecific(m_ptkey, reinterpret_cast<void*>(ids)) != 0)
		return asagi::Grid::THREAD_ERROR;

	return asagi::Grid::SUCCESS;
}
