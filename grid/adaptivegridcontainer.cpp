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

#include "adaptivegridcontainer.h"

#include "grid/grid.h"

/**
 * @see GridContainer::GridContainer()
 */
grid::AdaptiveGridContainer::AdaptiveGridContainer(asagi::Grid::Type type,
	bool isArray, unsigned int hint, unsigned int levels)
	: GridContainer(type, isArray, hint, levels),
	m_hint(hint)
{
	// Fancy way to allocate an array without default constructor
	m_grids = multiGridAllocator.allocate(m_levels);
	for (unsigned int i = 0; i < m_levels; i++)
		new(m_grids+i) MultiGrid(*this);
	m_grids[0].setTopLevel();
	
	m_ids = 0;
}

/**
 * @see GridContainer::GridContainer()
 */
grid::AdaptiveGridContainer::AdaptiveGridContainer(unsigned int count,
		unsigned int blockLength[],
		unsigned long displacements[],
		asagi::Grid::Type types[],
		unsigned int hint, unsigned int levels)
	: GridContainer(count, blockLength, displacements, types, hint, levels),
	m_hint(hint)
{
	m_grids = multiGridAllocator.allocate(m_levels);
	for (unsigned int i = 0; i < m_levels; i++)
		new(m_grids+i) MultiGrid(*this);
	m_grids[0].setTopLevel();
	
	m_ids = 0;
}


grid::AdaptiveGridContainer::~AdaptiveGridContainer()
{
	for (unsigned int i = 0; i < m_levels; i++)
		multiGridAllocator.destroy(&m_grids[i]);
	multiGridAllocator.deallocate(m_grids, m_levels);
}

asagi::Grid::Error grid::AdaptiveGridContainer::setParam(const char* name,
	const char* value, unsigned int level)
{
	Error result;

	result = GridContainer::setParam(name, value, level);
	if (result != UNKNOWN_PARAM)
		return result;

	assert(level < m_levels);
	return m_grids[level].setParam(name, value);
}

asagi::Grid::Error grid::AdaptiveGridContainer::open(const char* filename,
	unsigned int level)
{
	Error result;
	
	result = GridContainer::open(filename, level);
	if (result != SUCCESS)
		return result;

	result = m_grids[level].open(filename);
	if (result != SUCCESS)
		return result;
	
	if (level == 0) {
		// Set min/max from level 0 grid
		m_minX = m_grids[0].getGrid(0)->getXMin();
		m_minY = m_grids[0].getGrid(0)->getYMin();
		m_minZ = m_grids[0].getGrid(0)->getZMin();
		
		m_maxX = m_grids[0].getGrid(0)->getXMax();
		m_maxY = m_grids[0].getGrid(0)->getYMax();
		m_maxZ = m_grids[0].getGrid(0)->getZMax();
	}
	
	return SUCCESS;
}

unsigned char grid::AdaptiveGridContainer::getByte3D(double x, double y, double z, unsigned int level)
{
	return getGrid(x, y, z, level)->getByte(x, y, z);
}

int grid::AdaptiveGridContainer::getInt3D(double x, double y, double z, unsigned int level)
{
	return getGrid(x, y, z, level)->getInt(x, y, z);
}

long grid::AdaptiveGridContainer::getLong3D(double x, double y, double z,
	unsigned int level)
{
	return getGrid(x, y, z, level)->getLong(x, y, z);
}

float grid::AdaptiveGridContainer::getFloat3D(double x, double y, double z,
	unsigned int level)
{
	return getGrid(x, y, z, level)->getFloat(x, y, z);
}

double grid::AdaptiveGridContainer::getDouble3D(double x, double y, double z,
	unsigned int level)
{
	return getGrid(x, y, z, level)->getDouble(x, y, z);
}

void grid::AdaptiveGridContainer::getBuf3D(void* buf, double x, double y, double z,
	unsigned int level)
{
	getGrid(x, y, z, level)->getBuf(buf, x, y, z);
}

bool grid::AdaptiveGridContainer::exportPng(const char* filename, unsigned int level)
{
	assert(level < m_levels);
	
	if (level != 0)
		// Sry, but we can not export subgrids ...
		return false;
	
	return m_grids[0].getGrid(0)->exportPng(filename);
}

unsigned long grid::AdaptiveGridContainer::getCounter(const char* name, unsigned int level)
{
	assert(level < m_levels);

	std::vector<grid::Grid*>::const_iterator grid;
	unsigned long counter = 0;

	for (grid = m_grids[level].begin(); grid < m_grids[level].end(); grid++)
		counter += (*grid)->getCounter(name);

	return counter;
}

/**
 * Get the grid with the best level of detail (but not better than level) for
 * (x,y,z).
 */
grid::Grid* grid::AdaptiveGridContainer::getGrid(double x, double y, double z,
	unsigned int level)
{
	std::vector<grid::Grid*>::const_iterator grid;
	
	assert(level < m_levels);
	assert(m_minX <= x && m_maxX >= x && m_minY <= y && m_maxY >= y
		&& m_minZ <= z && m_maxZ >= z);
	
	for (size_t i = level; i > 0; i--) {
		for (grid = m_grids[i].begin(); grid < m_grids[i].end(); grid++) {
			if ((*grid)->getXMin() <= x && (*grid)->getXMax() >= x
				&& (*grid)->getYMin() <= x && (*grid)->getYMax() >= y
				&& (*grid)->getXMin() <= x && (*grid)->getZMax() >= z)
				return *grid;
		}
	}
	
	return m_grids[0].getGrid(0);
}

/**
 * This allocator is used to (de)allocate any MultiGrid memory. This is a
 * workaround because MultiGrid does not have a default constructor.
 */
std::allocator<grid::MultiGrid>
	grid::AdaptiveGridContainer::multiGridAllocator;
