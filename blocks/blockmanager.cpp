/**
 * @file
 *  This file is part of ASAGI.
 * 
 *  ASAGI is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as
 *  published by the Free Software Foundation, either version 3 of
 *  the License, or  (at your option) any later version.
 *
 *  ASAGI is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with ASAGI.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Diese Datei ist Teil von ASAGI.
 *
 *  ASAGI ist Freie Software: Sie koennen es unter den Bedingungen
 *  der GNU Lesser General Public License, wie von der Free Software
 *  Foundation, Version 3 der Lizenz oder (nach Ihrer Option) jeder
 *  spaeteren veroeffentlichten Version, weiterverbreiten und/oder
 *  modifizieren.
 *
 *  ASAGI wird in der Hoffnung, dass es nuetzlich sein wird, aber
 *  OHNE JEDE GEWAEHELEISTUNG, bereitgestellt; sogar ohne die implizite
 *  Gewaehrleistung der MARKTFAEHIGKEIT oder EIGNUNG FUER EINEN BESTIMMTEN
 *  ZWECK. Siehe die GNU Lesser General Public License fuer weitere Details.
 *
 *  Sie sollten eine Kopie der GNU Lesser General Public License zusammen
 *  mit diesem Programm erhalten haben. Wenn nicht, siehe
 *  <http://www.gnu.org/licenses/>.
 * 
 * @copyright 2012 Sebastian Rettenberger <rettenbs@in.tum.de>
 */

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

void BlockManager::init(unsigned long maxBlocksPerNode,
	long handDiff)
{
	m_indexToBlock = new long[maxBlocksPerNode];
	for (unsigned long i = 0; i < maxBlocksPerNode; i++)
		m_indexToBlock[i] = -1;
	
	m_lru.init(maxBlocksPerNode, handDiff);
}

/**
 * @param block The global block id
 * @param[out] index If true, the local index of this block,
 *  otherwise the value is undefined
 * @return True if this block is stored, false otherwise
 */
bool BlockManager::getIndex(unsigned long block, unsigned long &index)
{
	unordered_map<unsigned long, unsigned long>::const_iterator value
		= m_blockToIndex.find(block);
		
	if (value == m_blockToIndex.end())
		return false;
	
	index = (*value).second;
	m_lru.access(index);
	
	return true;
}

/**
 * @param block The id of the new block
 * @param index The index, where the block should be saved
 * @return The id of the block that was deleted
 */
long BlockManager::getFreeIndex(unsigned long block, unsigned long &index)
{
	index = m_lru.getFree();
	long oldBlock = m_indexToBlock[index];
	
	if (oldBlock >= 0) {
		// This block is not empty
		// -> delete the old block
		
		m_blockToIndex.erase(oldBlock);
	}
	
	m_indexToBlock[index] = block;
	m_blockToIndex[block] = index;
	
	return oldBlock;
}
