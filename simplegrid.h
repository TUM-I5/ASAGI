#ifndef SIMPLEGRID_H
#define SIMPLEGRID_H

#include "grid.h"

#ifndef THREADSAFETY
#include <mutex>
#endif // THREADSAFETY
#include <unordered_map>

#include "blocks/blockmanager.h"

class SimpleGrid : public Grid
{
private:
	/** Blocks we are the master */
	unsigned char *masterData;
	
	/** Number of master blocks on each process */
	unsigned long masterBlockCount;
	
	/** Data we hold only temporary */
	unsigned char *slaveData;
	
	blocks::BlockManager blockManager;
	
	MPI_Win window;
	
#ifndef THREADSAFETY
	/**
	 * Lock slave memory
	 * @todo Use a shared mutex, to allow multiple readers
	 */
	std::mutex slave_mutex;
#endif // THREADSAFETY
public:
	SimpleGrid(GridContainer &container);
	virtual ~SimpleGrid();
	
protected:
	asagi::Grid::Error init();
	
	void getAt(void* buf, types::Type::converter_t converter,
		unsigned long x, unsigned long y = 0, unsigned long z = 0);
};

#endif // SIMPLEGRID_H
