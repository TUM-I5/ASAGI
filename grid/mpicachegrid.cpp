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
 * @copyright 2012-2013 Sebastian Rettenberger <rettenbs@in.tum.de>
 */

#include "mpicachegrid.h"

#include <cassert>
#include <malloc.h>
#include <stdlib.h>

#include "types/type.h"
#include "debug/dbg.h"

/**
 * @see StaticGrid::StaticGrid()
 */
grid::MPICacheGrid::MPICacheGrid(const GridContainer &container,
	unsigned int hint)
	: Grid(container, hint),
	  StaticGrid(container, hint),
	  LocalCacheGrid(container, hint)
{
	m_window = MPI_WIN_NULL;
}

grid::MPICacheGrid::~MPICacheGrid()
{
	if (m_window != MPI_WIN_NULL)
		MPI_Win_free(&m_window);
}

asagi::Grid::Error grid::MPICacheGrid::init()
{
	unsigned long blockSize = getTotalBlockSize();
	unsigned long masterBlockCount = getLocalBlockCount();
	asagi::Grid::Error error;
	
	// Create the local cache
	error = LocalCacheGrid::init();
	if (error != asagi::Grid::SUCCESS)
		return error;

	// Distribute the blocks
	error = StaticGrid::init();
	if (error != asagi::Grid::SUCCESS)
		return error;
	
	// Create the mpi window for distributed blocks
	if (MPI_Win_create(getData(),
		getType().getSize() * blockSize * masterBlockCount,
		getType().getSize(),
		MPI_INFO_NULL,
		getMPICommunicator(),
		&m_window) != MPI_SUCCESS)
		return asagi::Grid::MPI_ERROR;
	
	return asagi::Grid::SUCCESS;
}

void grid::MPICacheGrid::getAt(void* buf, types::Type::converter_t converter,
	unsigned long x, unsigned long y, unsigned long z)
{
	unsigned long block = getBlockByCoords(x, y, z);
	int remoteRank = getBlockRank(block);
	
	if (remoteRank == getMPIRank()) {
		// Nice, this is a block where we are the master
		StaticGrid::getAt(buf, converter, x, y, z);
		return;
	}
	
	// This function will call getBlock, if we need to transfer the block
	LocalCacheGrid::getAt(buf, converter, x, y, z);
}

/**
 * Transfer the block form the remote rank, that holds it
 */
void grid::MPICacheGrid::getBlock(unsigned long block,
	long oldBlock,
	unsigned long cacheIndex,
	unsigned char *cache)
{
	unsigned long blockSize = getTotalBlockSize();
	int remoteRank = getBlockRank(block);
	unsigned long offset = getBlockOffset(block);
	int mpiResult; NDBG_UNUSED(mpiResult);

	incCounter(perf::Counter::MPI);

	mpiResult = MPI_Win_lock(MPI_LOCK_SHARED, remoteRank,
		MPI_MODE_NOCHECK, m_window);
	assert(mpiResult == MPI_SUCCESS);

	mpiResult = MPI_Get(cache,
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
