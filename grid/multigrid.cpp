/**
 * @file
 *  This file is part of ASAGI (Source)
 *
 *  ASAGI (Source) is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  ASAGI (Source) is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with ASAGI (Source).  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Diese Datei ist Teil von ASAGI (Source).
 *
 *  ASAGI (Source) ist Freie Software: Sie koennen es unter den Bedingungen
 *  der GNU General Public License, wie von der Free Software Foundation,
 *  Version 3 der Lizenz oder (nach Ihrer Option) jeder spaeteren
 *  veroeffentlichten Version, weiterverbreiten und/oder modifizieren.
 *
 *  ASAGI (Source) wird in der Hoffnung, dass es nuetzlich sein wird, aber
 *  OHNE JEDE GEWAEHELEISTUNG, bereitgestellt; sogar ohne die implizite
 *  Gewaehrleistung der MARKTFAEHIGKEIT oder EIGNUNG FUER EINEN BESTIMMTEN
 *  ZWECK. Siehe die GNU General Public License fuer weitere Details.
 *
 *  Sie sollten eine Kopie der GNU General Public License zusammen mit diesem
 *  Programm erhalten haben. Wenn nicht, siehe <http://www.gnu.org/licenses/>.
 * 
 * @copyright 2012 Sebastian Rettenberger <rettenbs@in.tum.de>
 */

#include "multigrid.h"

#include "grid/grid.h"

#include "utils/stringutils.h"

#include <cstdlib>

/**
 * @param gridCreator The grid creator that should be used to create
 *  a new grid
 */
grid::MultiGrid::MultiGrid(GridCreator &gridCreator)
	: m_gridCreator(gridCreator),
	  m_topLevel(false),
	  m_nextOpen(0)
{
	m_grids.push_back(gridCreator.createGrid());
}

grid::MultiGrid::~MultiGrid()
{
	for (std::vector<Grid*>::const_iterator i = m_grids.begin();
		i < m_grids.end(); i++)
		delete *i;
}

/**
 * Accepts the following parameter:
 * @li @b multigrid-size
 *
 * @see asagi::Grid::setParam(const char*, const char*, unsigned int)
 */
asagi::Grid::Error grid::MultiGrid::setParam(const char* name, const char* value)
{

	if (strcmp(name, "multigrid-size") == 0) {
		unsigned int multiGridSize = atoi(value);
		if (multiGridSize <= m_grids.size())
			// User cannot decrease the grid container size
			// multigrid-size should only be set once anyway ...
			return asagi::Grid::INVALID_VALUE;
		if (multiGridSize > 1 && m_topLevel)
			// Top level grids can only consist of one grid
			return asagi::Grid::MULTIPLE_TOPGRIDS;

		// Create the new grids
		for (size_t i = m_grids.size(); i < multiGridSize; i++)
			m_grids.push_back(m_gridCreator.createGrid());

		return asagi::Grid::SUCCESS;
	}

	asagi::Grid::Error result = asagi::Grid::SUCCESS;

	for (size_t i = 0; i < m_grids.size(); i++) {
		std::string v(value);

		if (strcmp(name, "variable-name") == 0) {
			// Replace %d in the variable name with i
			// Allows the use to place all parts in one
			// netCDF file
			utils::StringUtils::replace(v, "%d", utils::StringUtils::toString(i));
		}

		asagi::Grid::Error error = m_grids[i]->setParam(name, v.c_str());

		// We only return the last result != SUCCESS
		// but that should be enough in most cases
		if (error != asagi::Grid::SUCCESS)
			result = error;
	}

	return result;
}

/**
 * Opens the netCDF file for the next grid. Should be called x times, where
 * x is the value set to "multigrid-size".
 */
asagi::Grid::Error grid::MultiGrid::open(const char* filename)
{
	assert(m_nextOpen < m_grids.size());

	return m_grids[m_nextOpen++]->open(filename);
}
