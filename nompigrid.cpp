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
 * @version \$Id$
 */

#include "nompigrid.h"

#include <cstdlib>

/**
 * @see Grid::Grid()
 */
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
