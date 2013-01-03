/**
 * @file
 *  This file is part of ASAGI
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

#include "passthroughgrid.h"

#include <algorithm>

/**
 * @see Grid::Grid()
 */
grid::PassThroughGrid::PassThroughGrid(const GridContainer &container,
		unsigned int hint)
	: Grid(container, hint),
	  m_mem(0L)
{

}

grid::PassThroughGrid::~PassThroughGrid()
{
	delete [] m_mem;
}

asagi::Grid::Error grid::PassThroughGrid::init()
{
	m_mem = new unsigned char[getType().getSize()];

	return asagi::Grid::SUCCESS;
}

void grid::PassThroughGrid::getAt(void* buf, types::Type::converter_t converter,
	unsigned long x, unsigned long y, unsigned long z)
{
	size_t pos[] = {x, y, z};
	size_t size[MAX_DIMENSIONS];

	// Load one value in each dimension
	std::fill_n(size, MAX_DIMENSIONS, 1);

	getType().load(getInputFile(), pos, size, m_mem);

	(getType().*converter)(m_mem, buf);
}
