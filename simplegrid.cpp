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

#include "simplegrid.h"

#include <cassert>
#include <malloc.h>
#include <stdlib.h>

#include "types/type.h"
#include "debug/dbg.h"

/**
 * @see Grid::Grid()
 */
SimpleGrid::SimpleGrid(GridContainer &container, unsigned int hint)
	: Grid(container, hint)
{
	m_masterData = 0L;
	m_slaveData = 0L;
	
	m_window = MPI_WIN_NULL;
}

SimpleGrid::~SimpleGrid()
{
	if (m_window != MPI_WIN_NULL)
		MPI_Win_free(&m_window);
	
	MPI_Free_mem(m_masterData);
	free(m_slaveData);
}

asagi::Grid::Error SimpleGrid::init()
{
	unsigned long blockSize = getBlockSize();
	unsigned long blockX, blockY, blockZ;
	unsigned long masterBlockCount = getLocalBlockCount();
	
	// Allocate memory for slave blocks
	m_slaveData = static_cast<unsigned char*>(
		malloc(getType().getSize() * blockSize * getBlocksPerNode()));
	m_blockManager.init(getBlocksPerNode(), getHandsDiff());

	if (MPI_Alloc_mem(getType().getSize() * blockSize * masterBlockCount,
		MPI_INFO_NULL, &m_masterData) != MPI_SUCCESS)
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
			&m_masterData[getType().getSize() * blockSize * i]);
	}
	
	// Create the mpi window for the master data
	if (MPI_Win_create(m_masterData,
		getType().getSize() * blockSize * masterBlockCount,
		getType().getSize(),
		MPI_INFO_NULL,
		getMPICommunicator(),
		&m_window) != MPI_SUCCESS)
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
		
		(getType().*converter)(&m_masterData[getType().getSize() *
			(blockSize * offset // jump to the correct block
			+ (z * getYBlockSize() + y) * getXBlockSize() + x) // correct value inside the block
			],
			buf);
		return;
	}
	
#ifdef THREADSAFETY
	std::lock_guard<std::mutex> lock(slave_mutex);
#endif // THREADSAFETY
	
	if (!m_blockManager.getIndex(block)) {
		// We do not have this block, transfer it first
		
		// Get index where we store the block
		m_blockManager.getFreeIndex(block);
		
		// Transfer data
		
		// I think we can use nocheck, because we only read
		// -> no conflicting locks
		// TODO check this
		mpiResult = MPI_Win_lock(MPI_LOCK_SHARED, remoteRank,
			MPI_MODE_NOCHECK, m_window);
		assert(mpiResult == MPI_SUCCESS);
		
		mpiResult = MPI_Get(&m_slaveData[getType().getSize() * blockSize * block],
			blockSize,
			getType().getMPIType(),
			remoteRank,
			offset * blockSize,
			blockSize,
			getType().getMPIType(),
			m_window);
		assert(mpiResult == MPI_SUCCESS);
		
		mpiResult = MPI_Win_unlock(remoteRank, m_window);
		assert(mpiResult == MPI_SUCCESS);
	}
		
	(getType().*converter)(&m_slaveData[getType().getSize() *
		(blockSize * block // correct block
		+ (z * getYBlockSize() + y) * getXBlockSize() + x) // correct value inside the block
		],
		buf);
}
