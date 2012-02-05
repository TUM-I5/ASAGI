#ifndef BLOCKS_BLOCKMANAGER_H
#define BLOCKS_BLOCKMANAGER_H

#include <unordered_map>

#include "lru.h"

namespace blocks
{
	class BlockManager
	{
	private:
		LRU m_lru;
		
		/**
		 * Maps from the local index of the block to the real block id.
		 * A block id < 0 means that this index is empty
		 */
		long *m_indexToBlock;
	
		/** Maps from the block id to the index where it is stored */
		std::unordered_map<unsigned long, unsigned long> m_blockToIndex;
	public:
		BlockManager();
		virtual ~BlockManager();
		
		void init(unsigned long maxBlocksPerNode);
		
		bool getIndex(unsigned long &block);
		
		unsigned long getFreeIndex(unsigned long block);
	};
}

#endif // BLOCKS_BLOCKMANAGER_H
