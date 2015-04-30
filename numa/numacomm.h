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

#ifndef NUMA_NUMACOMM_H
#define NUMA_NUMACOMM_H

#include "asagi.h"

#include "numa.h"
#include "threads/sync.h"

namespace numa
{

/**
 * Implements NUMA communication functions
 */
class NumaComm
{
private:
	/** NUMA detection */
	const Numa& m_numa;

	/** Synchronization mechanism */
	threads::Sync m_sync;

public:
	/**
	 * Use {@link Numa::createComm()} to create an instance of
	 * this class.
	 */
	NumaComm(const Numa& numa)
		: m_numa(numa)
	{
	}

	virtual ~NumaComm()
	{
	}

	/**
	 * @copydoc Numa::totalThreads
	 */
	unsigned int totalThreads() const
	{
		return m_numa.totalThreads();
	}

	/**
	 * @copydoc Numa::totalDomains
	 */
	unsigned int totalDomains() const
	{
		return m_numa.totalDomains();
	}

	/**
	 * @copydoc Numa::threadId
	 */
	unsigned int threadId() const
	{
		return m_numa.threadId();
	}

	/**
	 * @copydoc Numa::domainId
	 */
	unsigned int domainId() const
	{
		return m_numa.domainId();
	}

	/**
	 * Broadcast a value to all domain master.
	 * This is a collective operation among the NUMA masters.
	 *
	 * @param value The value that should be broadcasted
	 * @param rootDomain The thread (domainId of the thread) that holds
	 *  the value
	 */
	template<typename T>
	asagi::Grid::Error broadcast(T &value, unsigned int rootDomain = 0)
	{
		if (!m_sync.broadcast(value, totalDomains(), domainId(), rootDomain))
			return asagi::Grid::THREAD_ERROR;

		return asagi::Grid::SUCCESS;
	}

	/**
	 * Allocates memory for all NUMA domains. This is a collective
	 * operation among the NUMA masters.
	 *
	 * @param size Number of elements allocated for each NUMA domain
	 * @param[out] data Pointer to the allocated memory
	 */
	template<class Allocator, typename T>
	asagi::Grid::Error allocate(unsigned long size, T* &data)
	{
		if (domainId() == 0) {
			// Allocate the memory with the master thread
			asagi::Grid::Error err = Allocator::allocate(size * totalDomains(), data);
			if (err != asagi::Grid::SUCCESS)
				return err;
		}

		// Broadcast the pointer to all domains
		asagi::Grid::Error err = broadcast(data);
		if (err != asagi::Grid::SUCCESS)
			return err;

		// Get the local pointer
		data = &data[size * domainId()];

		return asagi::Grid::SUCCESS;
	}

	/**
	 * Frees the the memory allocated with {@link allocate()}
	 *
	 * @param data Pointer to the allocated memory
	 *
	 * @warning The caller has to make sure that is is only called from one thread.
	 */
	template<class Allocator, typename T>
	void free(T* data) const
	{
		Allocator::free(data);
	}

	/**
	 * Creates a copy of this communicator using {@link Numa::createComm}.
	 */
	NumaComm* copy() const
	{
		return m_numa.createComm();
	}
};

}

#endif // NUMA_NUMACOMM_H
