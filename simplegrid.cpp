#include "simplegrid.h"

#include <malloc.h>

#include "types/type.h"

SimpleGrid::SimpleGrid(Type type)
	: Grid(type)
{
	masterData = 0L;
	slaveData = 0L;
	
	window = MPI_WIN_NULL;
}

SimpleGrid::~SimpleGrid()
{
	if (window != MPI_WIN_NULL)
		MPI_Win_free(&window);
	
	MPI_Free_mem(masterData);
	free(slaveData);
}

bool SimpleGrid::init()
{
	unsigned long blockSize = getXBlockSize() * getYBlockSize();
	unsigned long blockX, blockY;
	
	masterBlockCount = (getBlockCount() + getMPISize() - 1) / getMPISize();

	MPI_Alloc_mem(getType()->getSize() * blockSize * masterBlockCount,
		MPI_INFO_NULL, &masterData);
	
	// Load the blocks from the file, which we control
	for (unsigned long i = 0; i < masterBlockCount; i++) {
		if (i + getMPIRank() * masterBlockCount >= getBlockCount())
			// Last process may controll less blocks
			break;
		
		// Get x and y coordinates of the block
		getBlockPos(i + getMPIRank() * masterBlockCount, blockX, blockY);
		
		// Get x and y coordinates of the first value in the block
		blockX = blockX * getXBlockSize();
		blockY = blockY * getYBlockSize();
		
		getType()->load(*getInputFile(),
			blockX, blockY,
			getXBlockSize(), getYBlockSize(),
			&masterData[getType()->getSize() * blockSize * i]);
	}
	
	// Create the mpi window for the master data
	if (MPI_Win_create(masterData,
		getType()->getSize() * blockSize * masterBlockCount,
		getType()->getSize(),
		MPI_INFO_NULL,
		getMPICommunicator(),
		&window) != MPI_SUCCESS)
		return false;
	
	// Allocate memory for slave blocks
	slaveData = static_cast<unsigned char*>(
		malloc(getType()->getSize() * blockSize * getBlocksPerNode()));
	blockManager.init(getBlocksPerNode());
	
	return true;
}

void SimpleGrid::getAt(unsigned long x, unsigned long y, void* buf,
	types::Type::converter_t converter)
{
	unsigned long blockSize = getXBlockSize() * getYBlockSize();
	unsigned long block = getBlockByCoords(x, y);
	int remoteRank;
	unsigned long remoteOffset;
	
	// Offset inside the block
	x %= getXBlockSize();
	y %= getYBlockSize();
	
	if ((block >= getMPIRank() * masterBlockCount)
		&& (block < (getMPIRank() + 1) * masterBlockCount)) {
		// Nice, this is a block where we are the master
		
		block -= getMPIRank() * masterBlockCount;
	
		(getType()->*converter)(&masterData[getType()->getSize() *
			(blockSize * block // jump to the correct block
			+ y * getXBlockSize() + x) // correct value inside the block
			],
			buf);
		return;
	}
	
#ifndef THREADSAFETY_DISABLED
	std::lock_guard<std::mutex> lock(slave_mutex);
#endif // THREADSAFETY_DISABLED
	
	if (!blockManager.getIndex(block)) {
		// We do not have this block, transfer it first
	
		// Where do we find the block?
		remoteRank = block / masterBlockCount;
		remoteOffset = block % masterBlockCount;
		
		// Index where we store the block
		block = blockManager.getFreeIndex(block);
		
		// Transfer data
		
		// I think we can use nocheck, because we only read
		// -> no conflicting locks
		// TODO check this
		MPI_Win_lock(MPI_LOCK_SHARED, remoteRank,
			MPI_MODE_NOCHECK, window);
		
		MPI_Get(&slaveData[getType()->getSize() * blockSize * block],
			blockSize,
			getType()->getMPIType(),
			remoteRank,
			remoteOffset * blockSize,
			blockSize,
			getType()->getMPIType(),
			window);
		
		MPI_Win_unlock(remoteRank, window);
	}
		
	(getType()->*converter)(&slaveData[getType()->getSize() *
		(blockSize * block // correct block
		+ y * getXBlockSize() + x) // correct value inside the block
		],
		buf);
}
