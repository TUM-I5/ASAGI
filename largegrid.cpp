/**
 * @file
 * 
 * @author Sebastian Rettenberger <rettenbs@in.tum.de>
 */

#include "largegrid.h"

#include <cstring>
#include <cstdlib>
#include <time.h>

LargeGrid::LargeGrid(GridContainer& container, unsigned int hint)
	: Grid(container, hint)
{
	m_data = 0L;
	m_dictionary = 0L;
	m_dictEntries = 2; // default
	
	m_dataWin = MPI_WIN_NULL;
	m_dictWin = MPI_WIN_NULL;
	m_dictLockWin = MPI_WIN_NULL;
}

LargeGrid::~LargeGrid()
{
	if (m_dataWin != MPI_WIN_NULL)
		MPI_Win_free(&m_dataWin);
	if (m_dictLockWin != MPI_WIN_NULL)
		MPI_Win_free(&m_dictLockWin);
	if (m_dictWin != MPI_WIN_NULL)
		MPI_Win_free(&m_dictWin);
	
	MPI_Free_mem(m_data);
	MPI_Free_mem(m_dictionary);
}

asagi::Grid::Error LargeGrid::init()
{
	unsigned long blockSize = getBlockSize();
	unsigned long dictCount = getLocalBlockCount();
	
	m_blockManager.init(getBlocksPerNode());
	
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
	
	if (MPI_Win_create(0, 0, 1, MPI_INFO_NULL, getMPICommunicator(),
		&m_dictLockWin) != MPI_SUCCESS)
		return asagi::Grid::MPI_ERROR;
	
	// Data
	if (MPI_Alloc_mem(getType().getSize() * blockSize * getBlocksPerNode(),
		MPI_INFO_NULL, &m_data) != MPI_SUCCESS)
		return asagi::Grid::MPI_ERROR;
	
	if (MPI_Win_create(m_data,
		getType().getSize() * blockSize * getBlocksPerNode(),
		getType().getSize(),
		MPI_INFO_NULL,
		getMPICommunicator(),
		&m_dataWin) != MPI_SUCCESS)
		return asagi::Grid::MPI_ERROR;
	
	srand(time(0));
	
	return asagi::Grid::SUCCESS;
}

void LargeGrid::getAt(void* buf, types::Type::converter_t converter,
	unsigned long x, unsigned long y, unsigned long z)
{
	unsigned long blockSize = getBlockSize();
	unsigned long block = getBlockByCoords(x, y, z);
	long oldBlock;
	int dictRank = getBlockRank(block), oldDictRank = -1, dataRank;
	unsigned long dictOffset = getBlockOffset(block), oldDictOffset,
		dataOffset;
	unsigned long* dictEntry = 0L;
	unsigned long* oldDictEntry = 0L;
	int mpiResult; NDBG_UNUSED(mpiResult);

	// Offset inside the block
	unsigned long offX = x % getXBlockSize();
	unsigned long offY = y % getYBlockSize();
	unsigned long offZ = z % getZBlockSize();
	
#ifndef THREADSAFETY
	std::lock_guard<std::mutex> lock(m_slave_mutex);
#endif // THREADSAFETY
	
	if (!m_blockManager.getIndex(block)) {
		// We do not have this block, transfer it first
		
		// Index where we store the block
		oldBlock = m_blockManager.getFreeIndex(block);
		
		if (oldBlock >= 0) {
			// We also need to propagete the information that we
			// no longer have the old block
			
			oldDictRank = getBlockRank(oldBlock);
			oldDictOffset = getBlockOffset(oldBlock);
		}
		
		mpiResult = MPI_Win_lock(MPI_LOCK_EXCLUSIVE, dictRank,
			0, m_dictLockWin);
		assert(mpiResult == MPI_SUCCESS);
		
		// Were is the information about this block stored
		if (dictRank == getMPIRank()) {
			// Information is stored local
			
			getBlockInfo(&m_dictionary[dictOffset * getDictLength()],
				block, dataRank, dataOffset);
			
			if (oldDictRank == dictRank) {
				// Old block also in this rank, delete the
				// information
				
				deleteBlockInfo(&m_dictionary[oldDictOffset
					* getDictLength()]);
			}
		} else {
			// Delete this after the window is unlocked!!
			dictEntry = new unsigned long[getDictLength()];
			
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
			
			if (oldDictRank == dictRank) {
				// Old block also on this rank
				oldDictEntry = new unsigned long[getDictLength()];
				
				mpiResult = MPI_Get(oldDictEntry,
					getDictLength(),
					MPI_UNSIGNED_LONG,
					oldDictRank,
					oldDictOffset * getDictLength(),
					getDictLength(),
					MPI_UNSIGNED_LONG,
					m_dictWin);
				assert(mpiResult == MPI_SUCCESS);
			}
			
			mpiResult = MPI_Win_unlock(dictRank, m_dictWin);
			assert(mpiResult == MPI_SUCCESS);
			
			getBlockInfo(dictEntry, block, dataRank, dataOffset);
			
			if (oldDictRank == dictRank)
				deleteBlockInfo(oldDictEntry);
			
			mpiResult = MPI_Win_lock(MPI_LOCK_EXCLUSIVE, dictRank,
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
			
			if (oldDictRank == dictRank) {
				// Old block also on this rank
				mpiResult = MPI_Put(oldDictEntry,
					getDictLength(),
					MPI_UNSIGNED_LONG,
					oldDictRank,
					oldDictOffset * getDictLength(),
					getDictLength(),
					MPI_UNSIGNED_LONG,
					m_dictWin);
			}
			
			mpiResult = MPI_Win_unlock(dictRank, m_dictWin);
			assert(mpiResult == MPI_SUCCESS);
			
			delete dictEntry;
			delete oldDictEntry; // is a NULL pointer if not used
		}
		
		if ((oldDictRank >= 0) && (oldDictRank != dictRank)) {
			// oldDictRank == dictRank was handled above
			
			mpiResult = MPI_Win_lock(MPI_LOCK_EXCLUSIVE, oldDictRank,
				0, m_dictLockWin);
			assert(mpiResult == MPI_SUCCESS);
			
			if (oldDictRank == getMPIRank()) {
				deleteBlockInfo(&m_dictionary[oldDictOffset
					* getDictLength()]);
			} else {
				oldDictEntry = new unsigned long[getDictLength()];
			
				mpiResult = MPI_Win_lock(MPI_LOCK_SHARED, oldDictRank,
					MPI_MODE_NOCHECK, m_dictWin);
				assert(mpiResult == MPI_SUCCESS);
				
				mpiResult = MPI_Get(oldDictEntry,
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
				
				deleteBlockInfo(oldDictEntry);
				
				mpiResult = MPI_Win_lock(MPI_LOCK_EXCLUSIVE, oldDictRank,
					MPI_MODE_NOCHECK, m_dictWin);
				assert(mpiResult == MPI_SUCCESS);
				
				mpiResult = MPI_Put(oldDictEntry,
					getDictLength(),
					MPI_UNSIGNED_LONG,
					oldDictRank,
					oldDictOffset * getDictLength(),
					getDictLength(),
					MPI_UNSIGNED_LONG,
					m_dictWin);
			
				mpiResult = MPI_Win_unlock(oldDictRank, m_dictWin);
				assert(mpiResult == MPI_SUCCESS);
				
				delete oldDictEntry;
			}
			
			mpiResult = MPI_Win_unlock(oldDictRank, m_dictLockWin);
			assert(mpiResult == MPI_SUCCESS);
		}
		
		// Lock local window
		mpiResult = MPI_Win_lock(MPI_LOCK_EXCLUSIVE, getMPIRank(),
			0, m_dataWin);
		assert(mpiResult == MPI_SUCCESS);
		
		if (dataRank < 0) {
			// Load the block form the netcdf file
			
			// We do not load from another rank, nobody can override
			// our block -> unlock dictWindow now
			mpiResult = MPI_Win_unlock(dictRank, m_dictLockWin);
			assert(mpiResult == MPI_SUCCESS);
			
			getType().load(getInputFile(),
				x, y, z,
				getXBlockSize(), getYBlockSize(), getZBlockSize(),
				&m_data[getType().getSize() * blockSize * block]);
		} else {
			// Transfer the block from the other rank
			
			// Lock remote window
			mpiResult = MPI_Win_lock(MPI_LOCK_SHARED, dataRank,
				0, m_dataWin);
			assert(mpiResult == MPI_SUCCESS);
		
			// Transfer data
			mpiResult = MPI_Get(&m_data[getType().getSize() * blockSize * block],
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
		
			// Now, nobody can steal our data -> unlock dictWindow
			mpiResult = MPI_Win_unlock(dictRank, m_dictLockWin);
			assert(mpiResult == MPI_SUCCESS);
		}
		
		// Unlock local window
		mpiResult = MPI_Win_unlock(getMPIRank(), m_dataWin);
		assert(mpiResult == MPI_SUCCESS);
	}
	
	(getType().*converter)(&m_data[getType().getSize() *
		(blockSize * block // correct block
		+ (offZ * getYBlockSize() + offY) * getXBlockSize() + offX) // correct value inside the block
		],
		buf);
}

/**
 * Get a rank and an offset from the entry and update it.
 * 
 * @param[out] rank The rank that contains the block
 * @param[out] offset The index  of the block in the data window
 */
void LargeGrid::getBlockInfo(unsigned long* dictEntry, unsigned long localOffset,
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
		// Dictionary full, delete oldest and shuffel the rest up
		
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
void LargeGrid::deleteBlockInfo(unsigned long* dictEntry)
{
	for (unsigned int i = 0; i < dictEntry[0]; i++) {
		if (dictEntry[i*2 + 1] == static_cast<unsigned int>(getMPIRank())) {
			memcpy(&dictEntry[i*2 + 1], &dictEntry[(i+1)*2 + 1],
				sizeof(unsigned long) * 2 * (dictEntry[0] - i - 1));
			break;
		}
	}
	
	dictEntry[0]--;
}

/**
 * @return The actual number of elements of single list
 */
unsigned long LargeGrid::getDictLength()
{
	return m_dictEntries * 2 + 1;
}