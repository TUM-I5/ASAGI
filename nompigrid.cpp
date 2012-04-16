#include "nompigrid.h"

#include <malloc.h>

NoMPIGrid::NoMPIGrid(GridContainer &container, unsigned int hint)
	: Grid(container, hint)
{
	m_data = 0L;
}

NoMPIGrid::~NoMPIGrid()
{
	free(m_data);
}

asagi::Grid::Error NoMPIGrid::init()
{
	unsigned long blockSize = getBlockSize();
	unsigned long blockX, blockY, blockZ;
	unsigned long masterBlockCount = getLocalBlockCount();
	
	m_data = static_cast<unsigned char*>(
		malloc(getType().getSize() * blockSize * masterBlockCount));
	
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
			&m_data[getType().getSize() * blockSize * i]);
	}
	
	return asagi::Grid::SUCCESS;
}

void NoMPIGrid::getAt(void* buf, types::Type::converter_t converter,
	long unsigned int x, long unsigned int y, long unsigned int z)
{
	unsigned long blockSize = getBlockSize();
	unsigned long block = getBlockByCoords(x, y, z);
	int remoteRank = getBlockRank(block); NDBG_UNUSED(remoteRank);
	unsigned long offset = getBlockOffset(block);
	
	// Offset inside the block
	x %= getXBlockSize();
	y %= getYBlockSize();
	z %= getZBlockSize();
	
	assert(remoteRank == getMPIRank());
		
	(getType().*converter)(&m_data[getType().getSize() *
		(blockSize * offset // jump to the correct block
		+ (z * getYBlockSize() + y) * getXBlockSize() + x) // correct value inside the block
		],
		buf);
}
