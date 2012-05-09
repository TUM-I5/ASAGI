/**
 * @file
 * 
 * @author Sebastian Rettenberger <rettensb@in.tum.de>
 */

#include "globaltest.h"
#include "tests.h"

#include "grid.h"

#include "simplegridcontainer.h"

class GridTest : public CxxTest::TestSuite
{
	SimpleGridContainer* c;
	Grid* grid;
public:
	void setUp(void)
	{
		// Set up a 1d grid
		c = new SimpleGridContainer(asagi::Grid::FLOAT);
		c->open("../"NC_1D);
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
};