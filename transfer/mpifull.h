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

#ifndef TRANSFER_MPIFULL_H
#define TRANSFER_MPIFULL_H

#include "asagi.h"

#include <cassert>

#include "mpi/mpicomm.h"
#include "types/type.h"

/**
 * @brief Transfer classes can copy blocks between MPI/NUMA domains
 */
namespace transfer
{

/**
 * Copies blocks between MPI processes assuming full storage
 */
class MPIFull
{
private:
	/** The MPI window used to communicate */
	MPI_Win m_window;

	/** Number of elements in one block */
	unsigned long m_blockSize;

	/** The type MPI type of an element */
	MPI_Datatype m_mpiType;

public:
	MPIFull()
		: m_window(MPI_WIN_NULL), m_blockSize(0), m_mpiType(MPI_DATATYPE_NULL)
	{
	}

	virtual ~MPIFull()
	{
		if (m_window != MPI_WIN_NULL)
			MPI_Win_free(&m_window);
	}

	/**
	 * Initialize the transfer class
	 *
	 * @param data Pointer to the local storage
	 * @param blockCount Number local blocks
	 * @param blockSize Number of elements in one block
	 * @param elementSize Size of one element (in bytes)
	 * @param comm The communicator object
	 */
	asagi::Grid::Error init(unsigned char* data,
			unsigned long blockCount,
			unsigned long blockSize,
			unsigned long elementSize,
			const types::Type &type,
			const mpi::MPIComm &comm)
	{
		m_blockSize = blockSize;

		m_mpiType = type.getMPIType();

		// Create the mpi window for distributed blocks
		if (MPI_Win_create(data,
			blockCount * blockSize * elementSize,
			elementSize,
			MPI_INFO_NULL,
			comm.comm(),
			&m_window) != MPI_SUCCESS)
			return asagi::Grid::MPI_ERROR;

		return asagi::Grid::SUCCESS;
	}

	void transfer(int remoteRank, unsigned long offset,
			unsigned char *cache)
	{
		int mpiResult; NDBG_UNUSED(mpiResult);

		mpiResult = MPI_Win_lock(MPI_LOCK_SHARED, remoteRank,
			MPI_MODE_NOCHECK, m_window);
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

}

#endif // TRANSFER_MPIFULL_H

