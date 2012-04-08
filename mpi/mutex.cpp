/**
 * @file
 * 
 * @author Sebastian Rettenberger <rettenbs@in.tum.de>
 */

#include "mutex.h"

#include "debug/dbg.h"

#include <cassert>

using namespace mpi;

/**
 * @param tag The tag, we should use for send and recv operations. Make sure
 *  the tag is unique within the communicator. For performance reason, the
 *  communicator set in {@link #init(MPI_Comm)} is not duplicated!
 */
Mutex::Mutex(int tag)
	: m_tag(tag)
{
	m_window = MPI_WIN_NULL;
	m_lock = 0L;
}

asagi::Grid::Error Mutex::init(MPI_Comm comm)
{
	size_t windowSize = 0;
	
	MPI_Comm_rank(comm, &m_rank);
	MPI_Comm_size(comm, &m_size);
	
	if (m_rank == 0)
		// Skip the rand initialization
		// We only want to distribute different mutexes among different
		// ranks. No need for true randomness
		m_homeRank = rand() % m_size;
	
	// Distribute to homeRank to all processes
	if(MPI_Bcast(&m_homeRank, 1, MPI_INT, 0, comm) != MPI_SUCCESS)
		return asagi::Grid::MPI_ERROR;
	
	if (m_rank == m_homeRank) {
		windowSize = sizeof(long) * m_size;
		
		if (MPI_Alloc_mem(windowSize,
			MPI_INFO_NULL, &m_lock) != MPI_SUCCESS)
			return asagi::Grid::MPI_ERROR;
		
		for (int i = 0; i < m_size; i++) {
			m_lock[i] = -1;
		}
	}
	
	if (MPI_Win_create(m_lock,
		windowSize,
		sizeof(long),
		MPI_INFO_NULL,
		comm,
		&m_window) != MPI_SUCCESS)
		return asagi::Grid::MPI_ERROR;
	
	// Create the mpi datatype
	int blockLenght[2] = {m_rank, m_size - m_rank - 1};
	int displacements[2] = {0, m_rank + 1};
	if (MPI_Type_indexed(2, blockLenght, displacements,
		MPI_LONG, &m_otherRanksType) != MPI_SUCCESS)
		return asagi::Grid::MPI_ERROR;
	if (MPI_Type_commit(&m_otherRanksType) != MPI_SUCCESS)
		return asagi::Grid::MPI_ERROR;
	
	// Safe communicator for send/recv
	m_comm = comm;
	
	return asagi::Grid::SUCCESS;
}

Mutex::~Mutex()
{
	if (m_window != MPI_WIN_NULL)
		MPI_Win_free(&m_window);
	
	MPI_Free_mem(m_lock);
	
	MPI_Type_free(&m_otherRanksType);
}

void Mutex::acquire(unsigned long block)
{
	long val = block;
	long* lockCopy; lockCopy = new long[m_size - 1];
	int i;
	int mpiResult; NDBG_UNUSED(mpiResult);
	
	// add self to lock list
	mpiResult = MPI_Win_lock(MPI_LOCK_EXCLUSIVE, m_homeRank,
		0, m_window);
	assert(mpiResult == MPI_SUCCESS);
	
	mpiResult = MPI_Put(&val,
		1, MPI_LONG,
		m_homeRank, m_rank,
		1 , MPI_LONG,
		m_window);
	assert(mpiResult == MPI_SUCCESS);
	
	mpiResult = MPI_Get(lockCopy,
		m_size - 1, MPI_LONG,
		m_homeRank, 0,
		1, m_otherRanksType,
		m_window);
	assert(mpiResult == MPI_SUCCESS);
	
	mpiResult = MPI_Win_unlock(m_homeRank, m_window);
	
	/* check to see if lock is already held */
	for (i = 0; i < (m_size - 1); i ++) {
		if (lockCopy[i] == static_cast<long>(block))
			break;
	}
	
	if (i < (m_size - 1)) {
		// wait for notification from some other process
		mpiResult = MPI_Recv(0L, 0, MPI_BYTE,
			MPI_ANY_SOURCE , m_tag, m_comm,
			MPI_STATUS_IGNORE);
		assert(mpiResult == MPI_SUCCESS);
	}
}

void Mutex::release(unsigned long block)
{
	long val = -1;
	long* lockCopy; lockCopy = new long[m_size - 1];
	int mpiResult; NDBG_UNUSED(mpiResult);
	
	// remove self from waitlist
	mpiResult = MPI_Win_lock(MPI_LOCK_EXCLUSIVE, m_homeRank,
		0, m_window);
	assert(mpiResult == MPI_SUCCESS);
	
	mpiResult = MPI_Get(lockCopy,
		m_size - 1, MPI_LONG,
		m_homeRank, 0,
		1, m_otherRanksType,
		m_window);
	assert(mpiResult == MPI_SUCCESS);
	
	
	mpiResult = MPI_Put(&val,
		1, MPI_LONG,
		m_homeRank, m_rank,
		1, MPI_LONG,
		m_window);
	assert(mpiResult == MPI_SUCCESS);
	
	mpiResult = MPI_Win_unlock(m_homeRank, m_window);
	assert(mpiResult == MPI_SUCCESS);
	
	// find the next rank waiting for the lock (use round robin)
	for (int i = m_rank; i < (m_size - 1); i++) {
		if (lockCopy[i] == static_cast<long>(block)) {
			// Rank is of by one
			mpiResult = MPI_Send(0L, 0, MPI_BYTE,
				i+1, m_tag, m_comm);
			assert(mpiResult == MPI_SUCCESS);
			return;
		}
	}
	for (int i = 0; i < m_rank; i++) {
		if (lockCopy[i] == static_cast<long>(block)) {
			mpiResult = MPI_Send(0L, 0, MPI_BYTE,
				i, m_tag, m_comm);
			assert(mpiResult == MPI_SUCCESS);
			return;
		}
	}
}