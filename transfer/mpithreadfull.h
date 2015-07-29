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

#ifndef TRANSFER_MPITHREADFULL_H
#define TRANSFER_MPITHREADFULL_H

#ifdef ASAGI_NOMPI
#include "mpino.h"
#else // ASAGI_NOMPI
#include "asagi.h"

#include <cassert>
#include <mutex>

#include "utils/logger.h"

#include "mpi/commthread.h"
#include "mpi/mpicomm.h"
#include "types/type.h"
#endif // ASAGI_NOMPI

/**
 * @brief Transfer classes can copy blocks between MPI/NUMA domains
 */
namespace transfer
{

#ifdef ASAGI_NOMPI
/** No MPI transfers with MPI */
typedef MPINo MPIThreadFull;
#else // ASAGI_NOMPI

/**
 * Copies blocks between MPI processes using MPI windows and
 * assuming full storage
 */
class MPIThreadFull : private mpi::Receiver
{
private:
	/** Pointer to the data block */
	const unsigned char* m_data;

	/** Number of elements per block */
	unsigned long m_blockSize;

	/** Size of a single value in bytes */
	unsigned int m_typeSize;

	/** The NUMA domain ID */
	unsigned int m_numaDomainId;

	/** The MPI communicator used by this grid */
	MPI_Comm m_comm;

	/** The type MPI type of an element */
	MPI_Datatype m_mpiType;

	/** The tag used for communication */
	int m_tag;

public:
	MPIThreadFull()
		: m_data(0L),
		  m_blockSize(0), m_typeSize(0),
		  m_numaDomainId(0),
		  m_comm(MPI_COMM_NULL), m_mpiType(MPI_DATATYPE_NULL),
		  m_tag(-1)
	{
	}

	virtual ~MPIThreadFull()
	{
		if (m_numaDomainId == 0 && m_tag >= 0)
			mpi::CommThread::commThread.unregisterReceiver(m_tag);
	}

	/**
	 * Initialize the transfer class
	 *
	 * @param data Pointer to the local storage
	 * @param blockCount Number local blocks
	 * @param blockSize Number of elements in one block
	 * @param type The data type of the elements
	 * @param mpiComm The MPI communicator
	 * @param numaComm The NUMA communicator
	 */
	asagi::Grid::Error init(unsigned char* data,
			unsigned long blockCount,
			unsigned long blockSize,
			const types::Type &type,
			const mpi::MPIComm &mpiComm,
			numa::NumaComm &numaComm)
	{
		m_data = data;
		m_blockSize = blockSize;
		m_typeSize = type.size();
		m_numaDomainId = numaComm.domainId();
		m_comm = mpiComm.comm();
		m_mpiType = type.getMPIType();

		if (m_numaDomainId == 0) {
			asagi::Grid::Error err = mpi::CommThread::commThread
				.registerReceiver(m_comm, *this, m_tag);
			if (err != asagi::Grid::SUCCESS)
				return err;
		}

		asagi::Grid::Error err = numaComm.broadcast(m_tag);
		if (err != asagi::Grid::SUCCESS)
			return err;

		return asagi::Grid::SUCCESS;
	}

	/**
	 * Transfers a block via MPI
	 *
	 * @param remoteRank Id of the rank that stores the data
	 * @param offset Offset of the block on this rank
	 * @param cache Pointer to the local cache for this block
	 */
	void transfer(int remoteRank, unsigned long offset,
			unsigned char *cache)
	{
		int mpiResult; NDBG_UNUSED(mpiResult);

		mpi::CommThread::commThread.send(m_tag, remoteRank, offset);

		mpiResult = MPI_Recv(cache, m_blockSize, m_mpiType, remoteRank, 0, m_comm, MPI_STATUS_IGNORE);
		assert(mpiResult == MPI_SUCCESS);
	}

	void recv(int sender, unsigned long blockId)
	{
		// We get the local blockId

		int mpiResult; NDBG_UNUSED(mpiResult);

		mpiResult = MPI_Send(const_cast<unsigned char*>(&m_data[blockId*m_blockSize*m_typeSize]),
				m_blockSize, m_mpiType, sender, 0, m_comm);
		assert(mpiResult == MPI_SUCCESS);
	}
};

#endif // ASAGI_NOMPI

}

#endif // TRANSFER_MPITHREADFULL_H

