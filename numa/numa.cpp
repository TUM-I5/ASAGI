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

#ifndef ASAGI_NONUMA
#include <numa.h>
#endif // ASAGI_NONUMA
#ifndef __APPLE__
#include <sched.h>
#endif

#include "numa.h"
#include "numacomm.h"

numa::Numa::Numa()
	: m_totalThreads(1),
	  m_initialized(false),
	  m_keyError(false)
{
	if (pthread_key_create(&m_ptkey, 0L) != 0)
		// remember this error
		m_keyError = true;
}

numa::Numa::~Numa()
{
	pthread_key_delete(m_ptkey);
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
 * @param detectNumaDomains Set this to false to disable the NUMA detection.
 */
asagi::Grid::Error numa::Numa::registerThread(bool &masterThread, bool detectNumaDomains)
{
	if (m_initialized) {
		masterThread = m_masterThreads[threadId()];
		return asagi::Grid::SUCCESS;
	}

	// Get the NUMA domain
#ifndef __APPLE__
	int cpu = sched_getcpu();
	if (cpu < 0)
		return asagi::Grid::NUMA_ERROR;
#endif
#ifdef ASAGI_NONUMA
	int domain = 0;
#else // ASAGI_NONUMA
	int domain = numa_node_of_cpu(cpu);
	if (domain < 0)
		return asagi::Grid::NUMA_ERROR;
#endif // ASAGI_NONUMA

	// Lock all variables
	if (!m_syncThreads.startBarrier())
		return asagi::Grid::THREAD_ERROR;

	// Get thead id
	unsigned int threadId = m_syncThreads.waiting();
#if DEBUG_NUMA
	// Each thread gets its own domain
	domain = threadId;
#endif // DEBUG_NUMA

	// NUMA detection enabled?
	if (!detectNumaDomains)
		domain = 0;

	// Detect if we are the master thread
	if (m_masterThreads.size() <= threadId)
		m_masterThreads.resize(threadId+1);

	// Get the domain id from the NUMA domain
	unsigned int domainId;
	std::map<int, unsigned int>::const_iterator domIt
		= m_domains.find(domain);
	if (domIt == m_domains.end()) {
		// new domain
		domainId = m_domains.size();
		m_domains[domain] = domainId;

		// This is the master thread
		m_masterThreads[threadId] = masterThread = true;
	} else {
		domainId = domIt->second;

		// This is not the master thread
		masterThread = false;
	}

	// Last thread?
	if (m_syncThreads.waiting() >= m_totalThreads-1)
		m_initialized = true;

	// Wait for all threads
	if (!m_syncThreads.waitBarrier(m_totalThreads))
		return asagi::Grid::THREAD_ERROR;
	if (!m_syncThreads.endBarrier())
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

/**
 * Creates <b>one</b> NUMA communicator.
 * This is a collective operation among all NUMA domains
 *
 * @warning The caller is responsible for freeing the communicator.
 */
numa::NumaComm* numa::Numa::createComm() const
{
	NumaComm* comm;
	unsigned int current = domainId();

	if (current == 0)
		comm = new NumaComm(*this);

	if (!m_syncDomains.broadcast(comm, totalDomains(), current))
		return 0L;

	return comm;
}
