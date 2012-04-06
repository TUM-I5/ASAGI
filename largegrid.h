#ifndef LARGEGRID_H
#define LARGEGRID_H

#include "grid.h"

#include "blocks/blockmanager.h"
#include "mpi/mutex.h"

class LargeGrid : public Grid
{
private:
	/** All blocks we currently store */
	unsigned char* m_data;
	
	/** The window that holds the blocks */
	MPI_Win m_dataWin;
	
	/**
	 * Information about which blocks are currently stored on which node.
	 * <br>
	 * Structure: <br>
	 * This array consists of {@link #m_dictCount} elements.
	 * Each element stores information about one block. It is a list of
	 * node/offset pairs which indicate where the block is currently stored.
	 * The length of the list is determind by {@link #m_dictLength}. The
	 * actual length of the list is stored in the element before the list.
	 */
	unsigned long* m_dictionary;
	
	/** The number of lists */
	unsigned long m_dictCount;
	
	/**
	 * The number entries (rank/offset pairs) in a single list in the
	 * directory
	 */
	unsigned long m_dictEntries;
	
	/** The window is used to load dictionary information from other ranks */
	MPI_Win m_dictWin;
	
	blocks::BlockManager m_blockManager;
	
	/** Prevent access to the same block from multiple processes */
	mpi::Mutex m_globalMutex;
	
#ifdef THREADSAFETY
	/**
	 * Lock blockmanager
	 * @todo Use a shared mutex, to allow multiple readers
	 */
	std::mutex m_slave_mutex;
#endif // THREADSAFETY
public:
	LargeGrid(GridContainer& container,
		unsigned int hint = asagi::NO_HINT);
	virtual ~LargeGrid();
protected:
	asagi::Grid::Error init();
	
	bool keepFileOpen()
	{
		return true;
	}
    
	void getAt(void* buf, types::Type::converter_t converter,
		unsigned long x, unsigned long y = 0, unsigned long z = 0);
	
private:
	void getBlockInfo(unsigned long* dictEntry, unsigned long localOffset,
		int &rank, unsigned long &offset);
	void deleteBlockInfo(unsigned long* dictEntry);
	unsigned long getDictLength();
};

#endif // LARGEGRID_H
