#ifndef SIMPLEGRID_H
#define SIMPLEGRID_H

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
public:
	SimpleGrid(Type type = FLOAT);
	virtual ~SimpleGrid();
	
protected:
	bool init();
	
	void* getAt(unsigned long x, unsigned long y);
};

#endif // SIMPLEGRID_H
