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
 * @copyright 2012-2013 Sebastian Rettenberger <rettenbs@in.tum.de>
 */

#include "localcachegrid.h"

/**
 * @see Grid::Grid()
 */
grid::LocalCacheGrid::LocalCacheGrid(const GridContainer &container,
		unsigned int hint,
		const allocator::Allocator<unsigned char> &allocator)
	: Grid(container, hint),
	  m_cache(0L),
	  m_allocator(allocator)
{

}

grid::LocalCacheGrid::~LocalCacheGrid()
{
	m_allocator.free(m_cache);
}

asagi::Grid::Error grid::LocalCacheGrid::init()
{
	unsigned long blockSize = getTotalBlockSize();
	asagi::Grid::Error error;

	// Allocate memory for cache
	error = m_allocator.allocate(getType().getSize() * blockSize * getBlocksPerNode(), m_cache);
	if (error != asagi::Grid::SUCCESS)
		return error;

	m_blockManager.init(getBlocksPerNode(), getHandsDiff());

	return asagi::Grid::SUCCESS;
}

void grid::LocalCacheGrid::getAt(void* buf, types::Type::converter_t converter,
	unsigned long x, unsigned long y, unsigned long z)
{
	unsigned long blockSize = getTotalBlockSize();
	unsigned long block = getBlockByCoords(x, y, z);
	unsigned long index;

#ifdef THREADSAFETY
	std::lock_guard<std::mutex> lock(m_slaveMutex);
#endif // THREADSAFETY

	if (!m_blockManager.getIndex(block, index)) {
		// We do not have this block, transfer it first

		// Get index where we store the block
		long oldBlock = m_blockManager.getFreeIndex(block, index);

		// Get the block
		getBlock(block, oldBlock, index,
			&m_cache[getType().getSize() * blockSize * index]);

	}

	// Offset inside the block
	x %= getBlockSize(0);
	y %= getBlockSize(1);
	z %= getBlockSize(2);

	(getType().*converter)(&m_cache[getType().getSize() *
		(blockSize * index // correct block
		+ (z * getBlockSize(1) + y) * getBlockSize(0) + x) // correct value inside the block
		],
		buf);
}

/**
 * Gets a block and puts it in the cache
 *
 * @param block The block id
 * @param oldBlock If greater 0, the id of the block that was deleted
 * @param cacheIndex The position of the block inside the cache
 * @param cache The memory location where the block should be stored
 */
void grid::LocalCacheGrid::getBlock(unsigned long block,
	long oldBlock,
	unsigned long cacheIndex,
	unsigned char *cache)
{
	incCounter(perf::Counter::FILE);

	// TODO change this function so it does not get the block id
	// but the block coordinates
	size_t pos[MAX_DIMENSIONS];
	getBlockPos(block, pos);

	for (int i = 0; i < MAX_DIMENSIONS; i++)
		pos[i] *= getBlockSize(i);

	getType().load(getInputFile(),
		pos, getBlockSize(),
		cache);
}
