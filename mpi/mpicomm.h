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

#ifndef MPI_MPICOMM_H
#define MPI_MPICOMM_H

#include <mpi.h>

namespace mpi
{

/**
 * Small wrapper around the MPI communicator
 */
class MPIComm
{
private:
	/** The communicator */
	MPI_Comm m_comm;

	/** My rank */
	int m_rank;

	/** Total number of ranks */
	int m_size;

public:
	MPIComm()
		: m_comm(MPI_COMM_NULL),
		  m_rank(0), m_size(1)
	{ }

	virtual ~MPIComm()
	{
		if (m_comm != MPI_COMM_NULL)
			MPI_Comm_free(&m_comm);
	}

	/**
	 * Initialize the communicator
	 */
	asagi::Grid::Error init(MPI_Comm comm)
	{
		if (MPI_Comm_dup(comm, &m_comm) != MPI_SUCCESS)
			return asagi::Grid::MPI_ERROR;

		if (MPI_Comm_rank(m_comm, &m_rank) != MPI_SUCCESS)
			return asagi::Grid::MPI_ERROR;
		if (MPI_Comm_size(m_comm, &m_size) != MPI_SUCCESS)
			return asagi::Grid::MPI_ERROR;

		return asagi::Grid::SUCCESS;
	}

	/**
	 * @return The associated communicator
	 */
	MPI_Comm comm() const
	{
		return m_comm;
	}

	/**
	 * @return My rank
	 */
	int rank() const
	{
		return m_rank;
	}

	/**
	 * @return Size of the communicator
	 */
	int size() const
	{
		return m_size;
	}
};

}

#endif // MPI_MPICOMM_H
