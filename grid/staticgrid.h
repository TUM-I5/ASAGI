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

#ifndef GRID_STATICGRID_H
#define GRID_STATICGRID_H

#include "grid.h"

namespace grid
{

/**
 * This grid loads all (local) blocks into memory at initialization.
 * Neither does this class change the blocks nor does it fetch new blocks.
 * If you try to access values of a non-local block, the behavior is
 * undefined.
 * 
 * If compiled without MPI, all blocks are local.
 */
class StaticGrid : virtual public Grid
{
private:
	/** Local data cache */
	unsigned char* m_data;

public:
	StaticGrid(const GridContainer &container,
		unsigned int hint = asagi::Grid::NO_HINT);
	virtual ~StaticGrid();
	
protected:
	virtual asagi::Grid::Error init();
	
	virtual void getAt(void* buf, types::Type::converter_t converter,
		unsigned long x, unsigned long y = 0, unsigned long z = 0);

	/**
	 * Allocate the memory that holds the local data.
	 * Can be overwritten to use special allocation.
	 */
	virtual asagi::Grid::Error allocLocalMem(size_t size, unsigned char *&data)
	{
		data = new unsigned char[size];
		return asagi::Grid::SUCCESS;
	}

	/**
	 * Frees local data.
	 *
	 * @see allocLocalMem
	 */
	virtual void freeLocalMem(unsigned char *data)
	{
		delete [] data;
	}

	/**
	 * @return A pointer to the blocks
	 */
	unsigned char* getData()
	{
		return m_data;
	}
};

}

#endif // GRID_STATICGRID_H
