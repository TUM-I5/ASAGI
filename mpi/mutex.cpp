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
 * @copyright 2012-2015 Sebastian Rettenberger <rettenbs@in.tum.de>
 */

#include "mutex.h"

mpi::Mutex::Mutex()
	: m_comm(0L),
	  m_homeRank(-1),
	  m_lock(0L),
	  m_lockCopy(0L),
	  m_window(MPI_WIN_NULL),
	  m_otherRanksType(MPI_DATATYPE_NULL),
	  m_tag(0)
{
}

/**
 * Free the memory associated with this mutex and destroy the window
 */
mpi::Mutex::~Mutex()
{
	if (m_window != MPI_WIN_NULL)
		MPI_Win_free(&m_window);

	MPI_Free_mem(m_lock);
	delete [] m_lockCopy;

	MPI_Type_free(&m_otherRanksType);
}

/**
 * Initialize the mutex
 * 
 * @param comm The communicator used for this mutex. The communicator is
 *  <b>not</b> not duplicated. Make sure the tag set with the constructor is
 *  unique! 
 */
asagi::Grid::Error mpi::Mutex::init(const MPIComm& comm)
{
	m_comm = &comm;
	
	if (comm.rank() == 0)
		// Skip the rand initialization
		// We only want to distribute different mutexes among different
		// ranks. No need for true randomness
		m_homeRank = rand() % comm.size();
	
	// Distribute to homeRank to all processes
	if(MPI_Bcast(&m_homeRank, 1, MPI_INT, 0, comm.comm()) != MPI_SUCCESS)
		return asagi::Grid::MPI_ERROR;
	
	size_t windowSize = 0;

	// select the tag
	nextTagMutex.lock();
	if (MPI_Allreduce(MPI_IN_PLACE, &nextTag, 1, MPI_INT, MPI_MAX, comm.comm())
			!= MPI_SUCCESS)
	m_tag = nextTag++;
	nextTagMutex.unlock();

	if (comm.rank() == m_homeRank) {
		windowSize = sizeof(long) * comm.size();
		
		if (MPI_Alloc_mem(windowSize,
			MPI_INFO_NULL, &m_lock) != MPI_SUCCESS)
			return asagi::Grid::MPI_ERROR;
		
		for (int i = 0; i < comm.size(); i++) {
			m_lock[i] = -1;
		}
	}
	
	if (MPI_Win_create(m_lock,
		windowSize,
		sizeof(long),
		MPI_INFO_NULL,
		comm.comm(),
		&m_window) != MPI_SUCCESS)
		return asagi::Grid::MPI_ERROR;
	
	// Create the mpi datatype
	int blockLenght[2] = {comm.rank(), comm.size() - comm.rank() - 1};
	int displacements[2] = {0, comm.rank() + 1};
	if (MPI_Type_indexed(2, blockLenght, displacements,
		MPI_LONG, &m_otherRanksType) != MPI_SUCCESS)
		return asagi::Grid::MPI_ERROR;
	if (MPI_Type_commit(&m_otherRanksType) != MPI_SUCCESS)
		return asagi::Grid::MPI_ERROR;
	
	// Used to fetch the lock array
	m_lockCopy = new long[comm.size()-1];

	return asagi::Grid::SUCCESS;
}

/** The next tag the should be used */
int mpi::Mutex::nextTag = 0;

/** Mutex to lock the access to {@link nextTag} */
threads::Mutex mpi::Mutex::nextTagMutex;
