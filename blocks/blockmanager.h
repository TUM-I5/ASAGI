/**
 * @file
 *  This file is part of ASAGI.
 * 
 *  ASAGI is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  ASAGI is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with ASAGI.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Diese Datei ist Teil von ASAGI.
 *
 *  ASAGI ist Freie Software: Sie koennen es unter den Bedingungen
 *  der GNU General Public License, wie von der Free Software Foundation,
 *  Version 3 der Lizenz oder (nach Ihrer Option) jeder spaeteren
 *  veroeffentlichten Version, weiterverbreiten und/oder modifizieren.
 *
 *  ASAGI wird in der Hoffnung, dass es nuetzlich sein wird, aber
 *  OHNE JEDE GEWAEHELEISTUNG, bereitgestellt; sogar ohne die implizite
 *  Gewaehrleistung der MARKTFAEHIGKEIT oder EIGNUNG FUER EINEN BESTIMMTEN
 *  ZWECK. Siehe die GNU General Public License fuer weitere Details.
 *
 *  Sie sollten eine Kopie der GNU General Public License zusammen mit diesem
 *  Programm erhalten haben. Wenn nicht, siehe <http://www.gnu.org/licenses/>.
 * 
 * @copyright 2012 Sebastian Rettenberger <rettenbs@in.tum.de>
 */

#ifndef BLOCKS_BLOCKMANAGER_H
#define BLOCKS_BLOCKMANAGER_H

#include "lru.h"

#include <unordered_map>

/**
 * @brief Algorithms to handle the local block cache
 */
namespace blocks
{

/**
 * @brief Controls a list of blocks
 * 
 * It does not store the blocks itself, it only handles the free spaces in the
 * list and maps between block index and list position.
 */
class BlockManager
{
private:
	/** Algorithm we use to delete old blocks */
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
	
	/**
	 * @param maxBlocksPerNode The maximum number of blocks stored on this
	 *  node
	 * @param handDiff Difference between the two hands in the
	 *  2-handed clock algorithm
	 */
	void init(unsigned long maxBlocksPerNode,
		long handDiff = -1);
	
	bool getIndex(unsigned long block, unsigned long &index);
	
	long getFreeIndex(unsigned long block, unsigned long &index);
};

}

#endif // BLOCKS_BLOCKMANAGER_H
