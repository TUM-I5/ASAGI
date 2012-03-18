/**
 * @file
 * 
 * @author Sebastian Rettenberger <rettensb@in.tum.de>
 */

#include "globaltest.h"

#include "grid.h"

class GridTest : public CxxTest::TestSuite
{
public:
	void setUp(void)
	{
		// We do not need MPI for the tests at the moment
		//MPIHelper::setUp();
	}
	
	void tearDown(void)
	{
		// We do not need MPI for the tests at the moment
		//MPIHelper::tearDown();
	}
	
	void testSetParam(void)
	{
		GridContainer c(asagi::Grid::FLOAT);
		
		TS_ASSERT_EQUALS(c.setParam("x-block-size", "5"),
			asagi::Grid::SUCCESS);
		TS_ASSERT_EQUALS(c.m_grids[0]->m_blockSize[0], 5u);
		
		c.setParam("y-block-size", "7");
		TS_ASSERT_EQUALS(c.m_grids[0]->m_blockSize[1], 7u);
		
		c.setParam("z-block-size", "42");
		TS_ASSERT_EQUALS(c.m_grids[0]->m_blockSize[2], 42u);
	}
};