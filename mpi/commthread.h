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

#ifndef MPI_COMMTHREAD_H
#define MPI_COMMTHREAD_H

#include "asagi.h"

#include <mutex>
#include <unordered_map>
#include <thread>
#include <pthread.h>

#include "utils/logger.h"

#include "threads/mutex.h"

namespace mpi
{

/** The data type that is transfered via the communication thread */
typedef unsigned long message_t;
/** The corresponding MPI data type */
const MPI_Datatype MPI_MESSAGE = MPI_UNSIGNED_LONG;

/**
 * Interface for classes that can receive messages via the
 * communication thread
 */
class Receiver
{
public:
	virtual ~Receiver() {}

	/**
	 * Called from the communication thread when a message is received
	 *
	 * @param sender The sender rank of the message
	 * @param data The message
	 */
	virtual void recv(int sender, message_t data) = 0;
};

/**
 * Manages a communication thread
 */
class CommThread
{
private:
	/**
	 * Receiver description
	 */
	struct ReceiverDetail {
		/** The Receiver */
		Receiver* recv;
		/** The MPI Group */
		MPI_Group group;
	};

private:
	/** The communicator for the communication thread */
	MPI_Comm m_comm;

	/** The corresponding MPI group for communication thread */
	MPI_Group m_group;

	/** The rank of the process in the communicator */
	int m_rank;

	/** The communication thread */
	pthread_t m_thread;

	/** Tag used for the next registering receiver */
	int m_nextTag;

	/** The map of all registered receivers */
	std::unordered_map<int, ReceiverDetail> m_receiver;

	/** Lock used for thread safety */
	threads::Mutex m_lock;

private:
	CommThread()
		: m_comm(MPI_COMM_NULL), m_group(MPI_GROUP_NULL), m_rank(0),
		  m_thread(pthread_self()),
		  m_nextTag(0)
	{}

public:
	~CommThread()
	{}

	/**
	 * Initializes the communication rank
	 */
	asagi::Grid::Error init(int schedCPU, MPI_Comm comm = MPI_COMM_WORLD)
	{
		// Lock this class
		std::lock_guard<threads::Mutex> lock(m_lock);

		// Make sure the communication thread is not initialized
		int initialized = m_comm != MPI_COMM_NULL;
		if (MPI_Allreduce(MPI_IN_PLACE, &initialized, 1, MPI_INT, MPI_LOR, comm)
				!= MPI_SUCCESS)
			return asagi::Grid::MPI_ERROR;

		if (initialized)
			return asagi::Grid::ALREADY_INITIALIZED;

		// Setup the communicator
		if (MPI_Comm_dup(comm, &m_comm) != MPI_SUCCESS)
			return asagi::Grid::MPI_ERROR;
		if (MPI_Comm_group(m_comm, &m_group) != MPI_SUCCESS)
			return asagi::Grid::MPI_ERROR;

		if (MPI_Comm_rank(m_comm, &m_rank))
			return asagi::Grid::MPI_ERROR;

		// Start the thread
		if (pthread_create(&m_thread, 0L, &commThreadHandler, 0L) != 0)
			return asagi::Grid::THREAD_ERROR;

		// Pin the thread
		int numCPUs = std::thread::hardware_concurrency();
		if (schedCPU < numCPUs && -schedCPU <= numCPUs) {
			// Only pin if a valid CPU id is given
			cpu_set_t cpuset;
			CPU_ZERO(&cpuset);

			if (schedCPU >= 0)
				CPU_SET(schedCPU, &cpuset);
			else
				CPU_SET(numCPUs - schedCPU, &cpuset);

			pthread_setaffinity_np(m_thread, sizeof(cpu_set_t), &cpuset);
		} else
			logInfo(m_rank) << "ASAGI: Invalid CPU id" << schedCPU
				<< "The communication thread will not be pined.";

		return asagi::Grid::SUCCESS;
	}

	/**
	 * Kills the communication thread
	 */
	void finialize()
	{
		// Lock this class (just the be sure)
		std::lock_guard<threads::Mutex> lock(m_lock);

		// Send exit signal
		message_t data = 0;
		MPI_Request request; // Use asynchronous send to work around a deadlock with Intel MPI
		MPI_Isend(&data, 1, MPI_MESSAGE, m_rank, EXIT_TAG, m_comm, &request);

		int done = 0;
		while (!done)
			MPI_Test(&request, &done, MPI_STATUS_IGNORE);

		// Wait for thread to finish
		pthread_join(m_thread, 0L);

		// Rest the nextTag and receiver
		m_nextTag = 0;
		if (!m_receiver.empty()) {
			logWarning() << "ASAGI: Not all receivers are removed correctly.";
			m_receiver.clear();
		}

		// Free the communicator
		MPI_Comm_free(&m_comm);
	}

	/**
	 * Registers a new receiver that wants to communicate using the
	 * communication thread.
	 *
	 * @param comm The MPI communicator used by the grid
	 * @param receiver The receiver class handling incoming messages
	 * @param[out] tag The tag identifying this grid
	 *
	 * @return SUCCESS or an ASAGI error
	 */
	asagi::Grid::Error registerReceiver(MPI_Comm comm, Receiver &receiver, int &tag)
	{
		// Lock this class (just the be sure)
		std::lock_guard<threads::Mutex> lock(m_lock);

		if (m_comm == MPI_COMM_NULL)
			return asagi::Grid::NOT_INITIALIZED;

		// Compute the tag
		if (MPI_Allreduce(&m_nextTag, &tag, 1, MPI_INT, MPI_MAX, comm) != MPI_SUCCESS)
			return asagi::Grid::MPI_ERROR;

		m_nextTag = tag+1;

		// Save the receiver
		ReceiverDetail detail;
		detail.recv = &receiver;
		MPI_Comm_group(comm, &detail.group);
		m_receiver[tag] = detail;

		return asagi::Grid::SUCCESS;
	}

	/**
	 * Remove a receiver from the communication thread
	 */
	void unregisterReceiver(int tag)
	{
		// Send unregister signal
		// We do this via the thread to make sure all messages in the
		// that might concern this receiver are received
		m_lock.lock(); // The lock is released by the communication thread

		message_t data = tag; // Tag is provided in the data field
		MPI_Send(&data, 1, MPI_MESSAGE, m_rank, UNREG_TAG, m_comm);
	}

	/**
	 * Send a message
	 *
	 * @param tag The tag identifying the receiver
	 * @param recv The rank of the receiver
	 * @param data The data that should be transfered
	 */
	void send(int tag, int recv, message_t data)
	{
		std::unordered_map<int, ReceiverDetail>::const_iterator it = m_receiver.find(tag);
		if (it == m_receiver.end())
			logWarning() << "ASAGI: Sending message from unregistered tag" << tag;
		else {
			int recvRank;
			MPI_Group_translate_ranks(it->second.group, 1, &recv, m_group, &recvRank);
			MPI_Send(&data, 1, MPI_MESSAGE, recvRank, tag, m_comm);
		}
	}

private:
	/**
	 * Main function of the communication thread
	 */
	static void* commThreadHandler(void *p)
	{
		while (true) {
			message_t data;
			MPI_Status status;

			MPI_Recv(&data, 1, MPI_MESSAGE, MPI_ANY_SOURCE, MPI_ANY_TAG, commThread.m_comm, &status);

			if (status.MPI_TAG == EXIT_TAG)
				break;

			if (status.MPI_TAG == UNREG_TAG) {
				commThread.m_receiver.erase(data);
				commThread.m_lock.unlock(); // Release the lock from unregisterReceiver
				continue;
			}

			std::unordered_map<int, ReceiverDetail>::const_iterator it = commThread.m_receiver.find(status.MPI_TAG);
			if (it == commThread.m_receiver.end())
				logWarning() << "ASAGI: Received unregistered message with tag" << status.MPI_TAG;
			else {
				int senderRank;
				MPI_Group_translate_ranks(commThread.m_group, 1, &status.MPI_SOURCE, it->second.group, &senderRank);
				it->second.recv->recv(senderRank, data);
			}
		}

		return 0L;
	}

public:
	static CommThread commThread;

private:
	/** Tag used to "kill" the communication thread */
	const static int EXIT_TAG = 32767;

	/** Tag used to unregister a receiver */
	const static int UNREG_TAG = 32767 - 1;
	// 32767 has to be supported by all MPI implementations
	// according to the MPI Standard
};

}

#endif // MPI_COMMTHREAD_H
