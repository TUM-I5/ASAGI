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
 * @copyright 2012-2015 Sebastian Rettenberger <rettenbs@in.tum.de>
 */

#include "globaltest.h"
#include "testdefines.h"

#include "grid/grid.h"
#include "grid/simplecontainer.h"
#include "grid/level/passthrough.h"
#include "types/basictype.h"

class PassThroughTest : public CxxTest::TestSuite
{
	grid::Grid* c;
	grid::level::PassThrough<magic::NullType, magic::NullType, types::BasicType<float>>* grid;
public:
	void setUp(void)
	{
		// Set up a 1d grid
		c = new grid::Grid(asagi::Grid::FLOAT);
		c->setParam("GRID", "PASS_THROUGH");
		c->open("../../../" NC_1D);
		grid = &dynamic_cast<grid::SimpleContainer<grid::level::PassThrough<magic::NullType,
					magic::NullType, types::BasicType<float>>,
				magic::NullType, magic::NullType,
				types::BasicType<float>>*>(c->m_containers[0])->m_levels[0];

		TS_ASSERT(grid);
	}
	
	void tearDown(void)
	{
		delete c;
	}

	void testTypeSize(void)
	{
		TS_ASSERT_EQUALS(grid->typeSize(), sizeof(float));
	}

	void testNumaDomainId(void)
	{
		TS_ASSERT_EQUALS(grid->numaDomainId(), grid->numa().domainId());
	}

	void testMax(void)
	{
		TS_ASSERT_DELTA(grid->max(0), WIDTH-1 + 0.5, 0.0001);
	}

	void testDelta(void)
	{
		TS_ASSERT_DELTA(grid->delta(0), 1.0, 0.0001);
	}
};
