#include "blockmanager.h"

using namespace blocks;
using namespace std;

BlockManager::BlockManager()
{
	m_indexToBlock = 0L;
}

BlockManager::~BlockManager()
{
	delete [] m_indexToBlock;
}

void BlockManager::init(unsigned long maxBlocksPerNode)
{
	m_indexToBlock = new long[maxBlocksPerNode];
	for (unsigned long i = 0; i < maxBlocksPerNode; i++)
		m_indexToBlock[i] = -1;
	
	m_lru.init(maxBlocksPerNode);
}

/**
 * @param[in,out] block In: The global block id, Out, if true: the local index
 *  of this block. If false is returned, the variable is not changed
 * @return True if this block is stored, false otherwise
 */
bool BlockManager::getIndex(unsigned long &block)
{
	unordered_map<unsigned long, unsigned long>::const_iterator value
		= m_blockToIndex.find(block);
		
	if (value == m_blockToIndex.end())
		return false;
	
	block = (*value).second;
	m_lru.access(block);
	
	return true;
}

/**
 * @param block The id of the new block
 * @return The index, where the new block should be saved
 */
unsigned long BlockManager::getFreeIndex(unsigned long block)
{
	unsigned long index = m_lru.getFree();
	
	if (m_indexToBlock[index] >= 0) {
		// This block is not empty
		// -> delete the old block
		
		m_blockToIndex.erase(m_indexToBlock[index]);
	}
	
	m_indexToBlock[index] = block;
	m_blockToIndex[block] = index;
	
	return index;
}