#ifndef SIMPLEGRID_H
#define SIMPLEGRID_H

#include <mutex>
#include <unordered_map>

#include "blocks/blockmanager.h"

#include "grid.h"

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
	
	/**
	 * Lock slave memory
	 * @todo Use a shared mutex, to allow multiple readers
	 */
	std::mutex slave_mutex;
public:
	SimpleGrid(Type type = FLOAT);
	virtual ~SimpleGrid();
	
protected:
	bool init();
	
	void getAt(unsigned long x, unsigned long y, void* buf,
		types::Type::converter_t converter);
};

#endif // SIMPLEGRID_H
