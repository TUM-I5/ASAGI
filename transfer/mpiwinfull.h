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

#ifndef TRANSFER_MPIWINFULL_H
#define TRANSFER_MPIWINFULL_H

#ifdef ASAGI_NOMPI
#include "mpino.h"
#else // ASAGI_NOMPI
#include "asagi.h"

#include <cassert>
#include <mutex>

#include "mpi/mpicomm.h"
#include "mpi/lockassert.h"
#include "threads/mutex.h"
#include "types/type.h"
#endif // ASAGI_NOMPI

/**
 * @brief Transfer classes can copy blocks between MPI/NUMA domains
 */
namespace transfer
{

#ifdef ASAGI_NOMPI
/** No MPI transfers with MPI */
typedef MPINo MPIWinFull;
#else // ASAGI_NOMPI

/**
 * Copies blocks between MPI processes using MPI windows and
 * assuming full storage
 */
class MPIWinFull
{
private:
	/** The NUMA domain ID */
	unsigned int m_numaDomainId;

	/** The MPI window used to communicate */
	MPI_Win m_window;

	/** Mutex for the window */
	threads::Mutex *m_winMutex;

	/** Number of elements in one block */
	unsigned long m_blockSize;

	/** The type MPI type of an element */
	MPI_Datatype m_mpiType;

public:
	MPIWinFull()
		: m_numaDomainId(0), m_window(MPI_WIN_NULL), m_winMutex(0L),
		  m_blockSize(0), m_mpiType(MPI_DATATYPE_NULL)
	{
	}

	virtual ~MPIWinFull()
	{
		if (m_numaDomainId == 0 && m_window != MPI_WIN_NULL) {
			delete m_winMutex;

			std::lock_guard<mpi::Lock> lock(mpi::MPIComm::mpiLock);
			MPI_Win_free(&m_window);
		}
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
		m_numaDomainId = numaComm.domainId();
		m_blockSize = blockSize;
		m_mpiType = type.getMPIType();

		if (m_numaDomainId == 0) {
			unsigned int typeSize = type.size();

			m_winMutex = new threads::Mutex();

			std::lock_guard<mpi::Lock> lock(mpi::MPIComm::mpiLock);

			// Create the mpi window for distributed blocks
			if (MPI_Win_create(data,
					blockCount * blockSize * typeSize * numaComm.totalDomains(),
					typeSize,
					MPI_INFO_NULL,
					mpiComm.comm(),
					&m_window) != MPI_SUCCESS)
				return asagi::Grid::MPI_ERROR;
		}

		asagi::Grid::Error err = numaComm.broadcast(m_winMutex);
		if (err != asagi::Grid::SUCCESS)
			return err;

		err = numaComm.broadcast(m_window);
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

		std::lock_guard<mpi::Lock> lock(mpi::MPIComm::mpiLock);

#ifndef THREADSAFE_MPI
		// Lock the window to make sure no other thread interferes between
		// MPI_Win_lock and MPI_Win_unlock
		// Not required if we need to make MPI calls thread safe anyway
		std::lock_guard<threads::Mutex> winLock(*m_winMutex);
#endif // THREADSAFE_MPI

		mpiResult = MPI_Win_lock(MPI_LOCK_SHARED, remoteRank,
			ASAGI_MPI_MODE_NOCHECK, m_window);
		assert(mpiResult == MPI_SUCCESS);

		mpiResult = MPI_Get(cache,
			m_blockSize,
			m_mpiType,
			remoteRank,
			offset * m_blockSize,
			m_blockSize,
			m_mpiType,
			m_window);
		assert(mpiResult == MPI_SUCCESS);

		mpiResult = MPI_Win_unlock(remoteRank, m_window);
		assert(mpiResult == MPI_SUCCESS);
	}
};

#endif // ASAGI_NOMPI

}

#endif // TRANSFER_MPIWINFULL_H

