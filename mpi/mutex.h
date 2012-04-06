#ifndef MPI_MUTEX_H
#define MPI_MUTEX_H

#include <asagi.h>
#ifdef THREADSAFETY
#include <mutex>
#endif // THREADSAFETY

namespace mpi {

/**
 * See "Implementing Byte-Range Locks Using MPI One-Sided Communication" for
 * details about this algorithm.
 * 
 * @Warning Aquire and release are notthreadsafe at the moment. We do not need
 * this since the {@link blocks::BlockManager} is not threadsafe either.
 */
class Mutex
{
private:
	MPI_Comm m_comm;
	
	int m_rank;
	int m_size;
	
	/** Rank were the process array is stored */
	int m_homeRank;
	
	/** List of flags used for the locking algorithm */
	long* m_lock;
	
	MPI_Win m_window;
	
	/**
	 * Allows to access all elements from {@link #m_lock} without the
	 * current rank
	 */
	MPI_Datatype m_otherRanksType;
	
	/** The tag we use for send/recv */
	int m_tag;
public:
	Mutex();
	virtual ~Mutex();
	
	asagi::Grid::Error init(MPI_Comm comm);
	
	void acquire(unsigned long block);
	
	void release(unsigned long block);
private:
	/**
	 * Use a new tag for each mutex to distiguish between them.
	 * 
	 * @Warning This only works as long as were the only one in the
	 *  communicator using send/recv.
	 */
	static int m_nextTag;
#ifdef THREADSAFETY
	static std::mutex m_tagMutex;
#endif // THREADSAFETY
};

}

#endif // MPI_MUTEX_H
