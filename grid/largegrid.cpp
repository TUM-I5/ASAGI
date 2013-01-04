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

#include "largegrid.h"

#include <cstring>
#include <cstdlib>
#include <time.h>

/**
 * @param container The container, this grid belongs to
 * @param hint Optimazation hints
 * @param id A unique id within the communicator
 * 
 * @see Grid::Grid()
 */
grid::LargeGrid::LargeGrid(const GridContainer& container,
	unsigned int hint, unsigned int id)
	: Grid(container, hint),
	  LocalCacheGrid(container, hint),
	  m_globalMutex(id)
{
	m_dictionary = 0L;
	m_dictEntries = 2; // default
	
	m_dataWin = MPI_WIN_NULL;
	m_dictWin = MPI_WIN_NULL;
}

grid::LargeGrid::~LargeGrid()
{
	if (m_dataWin != MPI_WIN_NULL)
		MPI_Win_free(&m_dataWin);
	if (m_dictWin != MPI_WIN_NULL)
		MPI_Win_free(&m_dictWin);
	
	MPI_Free_mem(m_dictionary);
}

asagi::Grid::Error grid::LargeGrid::init()
{
	unsigned long blockSize = getTotalBlockSize();
	unsigned long dictCount = getLocalBlockCount();
	asagi::Grid::Error error;
	
	// Dictionary
	if (MPI_Alloc_mem(sizeof(unsigned long) * getDictLength() *
		dictCount, MPI_INFO_NULL, &m_dictionary) != MPI_SUCCESS)
		return asagi::Grid::MPI_ERROR;
	
	for (unsigned long i = 0; i < dictCount; i++)
		m_dictionary[i * getDictLength()] = 0;
	
	if (MPI_Win_create(m_dictionary,
		sizeof(unsigned long) * getDictLength() * dictCount,
		sizeof(unsigned long),
		MPI_INFO_NULL,
		getMPICommunicator(),
		&m_dictWin) != MPI_SUCCESS)
		return asagi::Grid::MPI_ERROR;
	
	// Initialize the cache
	error = LocalCacheGrid::init();
	if (error != asagi::Grid::SUCCESS)
		return error;
	
	if (MPI_Win_create(getCache(),
		getType().getSize() * blockSize * getBlocksPerNode(),
		getType().getSize(),
		MPI_INFO_NULL,
		getMPICommunicator(),
		&m_dataWin) != MPI_SUCCESS)
		return asagi::Grid::MPI_ERROR;
	
	srand(time(0L));
	
	// Init the global mutex
	return m_globalMutex.init(getMPICommunicator());
}

void grid::LargeGrid::getBlock(unsigned long block,
	long oldBlock,
	unsigned long cacheIndex,
	unsigned char *cache)
{
	int mpiResult; NDBG_UNUSED(mpiResult);

	// Local buffer for a dict entry
	unsigned long *dictEntry = new unsigned long[getDictLength()];

	// TODO Since we use the global mutex, it should be save to
	// access all windows in shared mode. The global mutex makes
	// sure we do not access the same block from different ranks
	// at the same time. However, I'm not sure about this.

	if (oldBlock >= 0) {
		// We need to propagate the information that we
		// no longer have the old block
		
		int oldDictRank = getBlockRank(oldBlock);
		unsigned long oldDictOffset = getBlockOffset(oldBlock);
		
		m_globalMutex.acquire(oldBlock);
		
		if (oldDictRank == getMPIRank()) {
			deleteBlockInfo(&m_dictionary[oldDictOffset
				* getDictLength()]);
		} else {
			
			mpiResult = MPI_Win_lock(MPI_LOCK_SHARED, oldDictRank,
				MPI_MODE_NOCHECK, m_dictWin);
			assert(mpiResult == MPI_SUCCESS);
			
			mpiResult = MPI_Get(dictEntry,
				getDictLength(),
				MPI_UNSIGNED_LONG,
				oldDictRank,
				oldDictOffset * getDictLength(),
				getDictLength(),
				MPI_UNSIGNED_LONG,
				m_dictWin);
			assert(mpiResult == MPI_SUCCESS);

			mpiResult = MPI_Win_unlock(oldDictRank, m_dictWin);
			assert(mpiResult == MPI_SUCCESS);
			
			deleteBlockInfo(dictEntry);
			
			mpiResult = MPI_Win_lock(MPI_LOCK_SHARED, oldDictRank,
				MPI_MODE_NOCHECK, m_dictWin);
			assert(mpiResult == MPI_SUCCESS);

			mpiResult = MPI_Put(dictEntry,
				getDictLength(),
				MPI_UNSIGNED_LONG,
				oldDictRank,
				oldDictOffset * getDictLength(),
				getDictLength(),
				MPI_UNSIGNED_LONG,
				m_dictWin);
			
			mpiResult = MPI_Win_unlock(oldDictRank, m_dictWin);
			assert(mpiResult == MPI_SUCCESS);
		}
		
		m_globalMutex.release(oldBlock);
	}

	// Transfer the block and update the dictionary
	int dictRank = getBlockRank(block), dataRank;
	unsigned long dictOffset = getBlockOffset(block), dataOffset;

	m_globalMutex.acquire(block);

	if (dictRank == getMPIRank()) {
		getBlockInfo(&m_dictionary[dictOffset * getDictLength()],
			cacheIndex, dataRank, dataOffset);
	} else {
		mpiResult = MPI_Win_lock(MPI_LOCK_SHARED, dictRank,
			MPI_MODE_NOCHECK, m_dictWin);
		assert(mpiResult == MPI_SUCCESS);

		mpiResult = MPI_Get(dictEntry,
			getDictLength(),
			MPI_UNSIGNED_LONG,
			dictRank,
			dictOffset * getDictLength(),
			getDictLength(),
			MPI_UNSIGNED_LONG,
			m_dictWin);
		assert(mpiResult == MPI_SUCCESS);

		mpiResult = MPI_Win_unlock(dictRank, m_dictWin);
		assert(mpiResult == MPI_SUCCESS);

		getBlockInfo(dictEntry, cacheIndex, dataRank, dataOffset);
		
		mpiResult = MPI_Win_lock(MPI_LOCK_SHARED, dictRank,
			MPI_MODE_NOCHECK, m_dictWin);
		assert(mpiResult == MPI_SUCCESS);

		mpiResult = MPI_Put(dictEntry,
			getDictLength(),
			MPI_UNSIGNED_LONG,
			dictRank,
			dictOffset * getDictLength(),
			getDictLength(),
			MPI_UNSIGNED_LONG,
			m_dictWin);
		assert(mpiResult == MPI_SUCCESS);
		
		mpiResult = MPI_Win_unlock(dictRank, m_dictWin);
		assert(mpiResult == MPI_SUCCESS);
	}

	if (dataRank < 0) {
		// Load the block form the netcdf file
		
		LocalCacheGrid::getBlock(block, oldBlock, cacheIndex, cache);
	} else {
		unsigned long blockSize = getTotalBlockSize();

		// Transfer the block from the other rank
		
		// Lock remote window
		mpiResult = MPI_Win_lock(MPI_LOCK_SHARED, dataRank,
			MPI_MODE_NOCHECK, m_dataWin);
		assert(mpiResult == MPI_SUCCESS);

		// Transfer data
		mpiResult = MPI_Get(cache,
			blockSize,
			getType().getMPIType(),
			dataRank,
			dataOffset * blockSize,
			blockSize,
			getType().getMPIType(),
			m_dataWin);
		assert(mpiResult == MPI_SUCCESS);

		// Unlock remote window
		mpiResult = MPI_Win_unlock(dataRank, m_dataWin);
		assert(mpiResult == MPI_SUCCESS);
	}
	
	m_globalMutex.release(block);

	delete [] dictEntry;
}

/**
 * Get a rank and an offset from the entry and update it.
 * 
 * @param dictEntry The entry in the dictionary
 * @param localOffset The offset, where the block is stored in the local
 *  window
 * @param[out] rank The rank that contains the block
 * @param[out] offset The index  of the block in the data window
 */
void grid::LargeGrid::getBlockInfo(unsigned long* dictEntry, unsigned long localOffset,
	int &rank, unsigned long &offset)
{
	unsigned long pos; // Position in the list, we will use
	
	if (dictEntry[0] == 0) {
		// Not (yet) in memory
		rank = -1;
	} else {
		pos = rand() % dictEntry[0]; // Use modulo since it is fast and random enough
		rank = dictEntry[pos*2 + 1];
		offset = dictEntry[pos*2 + 2];
	}
	
	if (dictEntry[0] >= m_dictEntries) {
		// Dictionary full, delete oldest and shuffle the rest up
		
		memcpy(&dictEntry[1], &dictEntry[3],
			sizeof(unsigned long) * 2 * (m_dictEntries - 1));
			
		dictEntry[0]--;
	}
	
	dictEntry[dictEntry[0]*2 + 1] = getMPIRank();
	dictEntry[dictEntry[0]*2 + 2] = localOffset;
	dictEntry[0]++;
}

/**
 * Remove the the current rank form the entry
 */
void grid::LargeGrid::deleteBlockInfo(unsigned long* dictEntry)
{
	unsigned int i;
	
	for (i = 0; i < dictEntry[0]; i++) {
		if (dictEntry[i*2 + 1] == static_cast<unsigned int>(getMPIRank())) {
			memcpy(&dictEntry[i*2 + 1], &dictEntry[(i+1)*2 + 1],
				sizeof(unsigned long) * 2 * (dictEntry[0] - i - 1));
			break;
		}
	}
	
	if (i < dictEntry[0])
		// Only substract the entry, if we have found it
		dictEntry[0]--;
}

/**
 * @return The actual number of elements of single list
 */
unsigned long grid::LargeGrid::getDictLength()
{
	return m_dictEntries * 2 + 1;
}
