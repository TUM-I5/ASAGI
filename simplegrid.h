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

#ifndef SIMPLEGRID_H
#define SIMPLEGRID_H

#include "grid.h"

#ifndef THREADSAFETY
#include <mutex>
#endif // THREADSAFETY
#include <unordered_map>

#include "blocks/blockmanager.h"

/**
 * Simple grid implentation, that distributes the grid at the beginning 
 * across all MPI tasks.
 */
class SimpleGrid : public Grid
{
private:
	/** Blocks we are the master */
	unsigned char *m_masterData;
	
	/** Data we hold only temporary */
	unsigned char *m_slaveData;
	
	/** BlockManager used to control the slave memory */
	blocks::BlockManager m_blockManager;
	
	/** MPI window for communication */
	MPI_Win m_window;
	
#ifdef THREADSAFETY
	/**
	 * Lock slave memory
	 * @todo Use a shared mutex, to allow multiple readers
	 */
	std::mutex slave_mutex;
#endif // THREADSAFETY
public:
	SimpleGrid(GridContainer &container, unsigned int hint = asagi::NO_HINT);
	virtual ~SimpleGrid();
	
protected:
	asagi::Grid::Error init();
	
	void getAt(void* buf, types::Type::converter_t converter,
		unsigned long x, unsigned long y = 0, unsigned long z = 0);
};

#endif // SIMPLEGRID_H

