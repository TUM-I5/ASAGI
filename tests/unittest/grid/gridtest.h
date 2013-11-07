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

#include "globaltest.h"
#include "tests.h"

#include "grid/grid.h"

#include "grid/simplegridcontainer.h"

class GridTest : public CxxTest::TestSuite
{
	grid::SimpleGridContainer* c;
	grid::Grid* grid;
public:
	void setUp(void)
	{
		// Set up a 1d grid
		c = new grid::SimpleGridContainer(asagi::Grid::FLOAT);
		c->open("../../" NC_1D);
		grid = c->m_grids[0];
	}
	
	void tearDown(void)
	{
		delete c;
	}
	
	void testSetParam(void)
	{
		TS_ASSERT_EQUALS(grid->setParam("x-block-size", "5"),
			asagi::Grid::SUCCESS);
		TS_ASSERT_EQUALS(grid->m_blockSize[0], 5u);
		
		grid->setParam("y-block-size", "7");
		TS_ASSERT_EQUALS(grid->m_blockSize[1], 7u);
		
		grid->setParam("z-block-size", "42");
		TS_ASSERT_EQUALS(grid->m_blockSize[2], 42u);
		
		TS_ASSERT_EQUALS(grid->setParam("block-cache-size", "100"),
			asagi::Grid::SUCCESS);
		TS_ASSERT_EQUALS(grid->m_blocksPerNode, 100);
	}

	void testGetXMax(void)
	{
		TS_ASSERT_EQUALS(c->getFloat1D(c->getXMax()), NC_WIDTH-1);
	}

	void testGetXDelta(void)
	{
		TS_ASSERT_DELTA(c->getXDelta(), 1.0, 0.001);
	}
};
