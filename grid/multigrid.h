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

#ifndef GRID_MULTIGRID_H
#define GRID_MULTIGRID_H

#include <asagi.h>

#include <cassert>
#include <vector>

namespace grid
{

class Grid;

/**
 * Interface that can create grids. The {@link MultiGrid} requires
 * such an interface because it creates a new grid for every call
 * to {@link Grid::open}.
 */
class GridCreator
{
public:
	virtual ~GridCreator() {}

	/**
	 * @return A new grid
	 */
	virtual Grid* createGrid() = 0;
};

/**
 * This class does not store a grid itself. It acts as a container for
 * several grids instead.
 */
class MultiGrid
{
private:
	/** The GridCreator we use to create new grids */
	GridCreator &m_gridCreator;

	/**
	 * If this class is a top level grid, it will only allow one
	 * {@link open} call
	 */
	bool m_topLevel;

	/** The list of grids that does the real work */
	std::vector<Grid*> m_grids;

	/** The next grid that should be used when calling {@link open} */
	unsigned int m_nextOpen;

public:
	MultiGrid(GridCreator &gridCreator);
	virtual ~MultiGrid();

	/**
	 * Convert this grid into a top level grid. Top level grids
	 * can only have one grid inside the container.
	 */
	void setTopLevel()
	{
		m_topLevel = true;
	}

	asagi::Grid::Error setParam(const char* name, const char* value);

	asagi::Grid::Error open(const char* filename);

	/**
	 * @return The grid the index i
	 */
	Grid* getGrid(unsigned int i)
	{
		assert(i < m_grids.size());
		return m_grids[i];
	}

	/**
	 * @return Read iterator that points to the beginning of the grids
	 */
	std::vector<Grid*>::const_iterator begin()
	{
		return m_grids.begin();
	}

	/**
	 * @return Read iterator that points to the last grid
	 */
	Grid* back()
	{
		return m_grids.back();
	}

	/**
	 * @return Read iterator that points to the end of the grids
	 */
	std::vector<Grid*>::const_iterator end()
	{
		return m_grids.end();
	}
};

}

#endif /* GRID_MULTIGRID_H */
