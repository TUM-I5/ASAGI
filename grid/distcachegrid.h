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

#ifndef GRID_DISTCACHEGRID_H
#define GRID_DISTCACHEGRID_H

#include "localcachegrid.h"

#include "mpi/mutex.h"

namespace grid
{

/**
 * Stores one grid. It maintains a directory where the current location
 * of blocks is stored. Blocks are only loaded dynamically when required.
 */
class DistCacheGrid : public LocalCacheGrid
{
private:
	/** The window that holds the blocks */
	MPI_Win m_dataWin;
	
	/**
	 * Information about which blocks are currently stored on which node.
	 * <br>
	 * Structure: <br>
	 * This array consists of {@link getLocalBlockCount()} elements.
	 * Each element stores information about one block. It is a list of
	 * node/offset pairs which indicate where the block is currently stored.
	 * The length of the list is determind by {@link m_dictEntries}. The
	 * actual length of the list is stored in the element before the list.
	 */
	unsigned long* m_dictionary;
	
	/**
	 * The number entries (rank/offset pairs) in a single list in the
	 * directory
	 */
	unsigned long m_dictEntries;
	
	/** The window is used to load dictionary information from other ranks */
	MPI_Win m_dictWin;
	
	/** Prevent access to the same block from multiple processes */
	mpi::Mutex m_globalMutex;
	
public:
	DistCacheGrid(const GridContainer& container,
		unsigned int hint = asagi::Grid::NO_HINT,
		unsigned int id = 0);
	virtual ~DistCacheGrid();
protected:
	asagi::Grid::Error init();

	void getBlock(unsigned long block,
		long oldBlock,
		unsigned long cacheIndex,
		unsigned char* cache);
	
private:
	void getBlockInfo(unsigned long* dictEntry, unsigned long localOffset,
		int &rank, unsigned long &offset);
	void deleteBlockInfo(unsigned long* dictEntry);
	unsigned long getDictLength();
};

}

#endif // GRID_DISTCACHEGRID_H
