#include "simplegrid.h"

#include <cassert>
#include <malloc.h>
#include <stdlib.h>

#include "types/type.h"
#include "debug/dbg.h"

SimpleGrid::SimpleGrid(GridContainer &container, unsigned int hint)
	: Grid(container, hint)
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

asagi::Grid::Error SimpleGrid::init()
{
	unsigned long blockSize = getBlockSize();
	unsigned long blockX, blockY, blockZ;
	unsigned long masterBlockCount = getLocalBlockCount();
	
	// Allocate memory for slave blocks
	slaveData = static_cast<unsigned char*>(
		malloc(getType().getSize() * blockSize * getBlocksPerNode()));
	blockManager.init(getBlocksPerNode(), getHandsDiff());

	if (MPI_Alloc_mem(getType().getSize() * blockSize * masterBlockCount,
		MPI_INFO_NULL, &masterData) != MPI_SUCCESS)
		return asagi::Grid::MPI_ERROR;
	
	// Load the blocks from the file, which we control
	for (unsigned long i = 0; i < masterBlockCount; i++) {
		if (getGlobalBlock(i) >= getBlockCount())
			// Last process(es) may controll less blocks
			break;
		
		// Get x, y and z coordinates of the block
		getBlockPos(getGlobalBlock(i),
			blockX, blockY, blockZ);
		
		// Get x, y and z coordinates of the first value in the block
		blockX = blockX * getXBlockSize();
		blockY = blockY * getYBlockSize();
		blockZ = blockZ * getZBlockSize();
		
		getType().load(getInputFile(),
			blockX, blockY, blockZ,
			getXBlockSize(), getYBlockSize(), getZBlockSize(),
			&masterData[getType().getSize() * blockSize * i]);
	}
	
	// Create the mpi window for the master data
	if (MPI_Win_create(masterData,
		getType().getSize() * blockSize * masterBlockCount,
		getType().getSize(),
		MPI_INFO_NULL,
		getMPICommunicator(),
		&window) != MPI_SUCCESS)
		return asagi::Grid::MPI_ERROR;
	
	return asagi::Grid::SUCCESS;
}

void SimpleGrid::getAt(void* buf, types::Type::converter_t converter,
	unsigned long x, unsigned long y, unsigned long z)
{
	unsigned long blockSize = getBlockSize();
	unsigned long block = getBlockByCoords(x, y, z);
	int remoteRank = getBlockRank(block);
	unsigned long offset = getBlockOffset(block);
	int mpiResult; NDBG_UNUSED(mpiResult);
	
	// Offset inside the block
	x %= getXBlockSize();
	y %= getYBlockSize();
	z %= getZBlockSize();
	
	if (remoteRank == getMPIRank()) {
		// Nice, this is a block where we are the master
		
		(getType().*converter)(&masterData[getType().getSize() *
			(blockSize * offset // jump to the correct block
			+ (z * getYBlockSize() + y) * getXBlockSize() + x) // correct value inside the block
			],
			buf);
		return;
	}
	
#ifdef THREADSAFETY
	std::lock_guard<std::mutex> lock(slave_mutex);
#endif // THREADSAFETY
	
	if (!blockManager.getIndex(block)) {
		// We do not have this block, transfer it first
		
		// Get index where we store the block
		blockManager.getFreeIndex(block);
		
		// Transfer data
		
		// I think we can use nocheck, because we only read
		// -> no conflicting locks
		// TODO check this
		mpiResult = MPI_Win_lock(MPI_LOCK_SHARED, remoteRank,
			MPI_MODE_NOCHECK, window);
		assert(mpiResult == MPI_SUCCESS);
		
		mpiResult = MPI_Get(&slaveData[getType().getSize() * blockSize * block],
			blockSize,
			getType().getMPIType(),
			remoteRank,
			offset * blockSize,
			blockSize,
			getType().getMPIType(),
			window);
		assert(mpiResult == MPI_SUCCESS);
		
		mpiResult = MPI_Win_unlock(remoteRank, window);
		assert(mpiResult == MPI_SUCCESS);
	}
		
	(getType().*converter)(&slaveData[getType().getSize() *
		(blockSize * block // correct block
		+ (z * getYBlockSize() + y) * getXBlockSize() + x) // correct value inside the block
		],
		buf);
}
