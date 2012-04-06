/**
 * @file
 * 
 * @author Sebastian Rettenberger <rettensb@in.tum.de>
 */

#include "globaltest.h"
#include "tests.h"

#include "blocks/blockmanager.h"

class ArrayTypeTest : public CxxTest::TestSuite
{
	blocks::BlockManager* manager;
public:
	void setUp(void)
	{
		manager = new blocks::BlockManager();
		manager->init(3);
	}
	
	void tearDown(void)
	{
		delete manager;
	}
	
	void testGetIndex(void)
	{
		unsigned long block;
		
		block = 5;
		TS_ASSERT(!manager->getIndex(block));
		TS_ASSERT_EQUALS(block, 5u);
		
		manager->getFreeIndex(block);
		block = 6;
		manager->getFreeIndex(block);
		block = 7;
		manager->getFreeIndex(block);
		
		block = 6;
		TS_ASSERT(manager->getIndex(block));
		TS_ASSERT_EQUALS(block, 1u);
		
		// Override first entry
		block = 8;
		manager->getFreeIndex(block);
		
		block = 8;
		TS_ASSERT(manager->getIndex(block));
		TS_ASSERT_EQUALS(block, 0u);
		
		block = 5;
		TS_ASSERT(!manager->getIndex(block));
		TS_ASSERT_EQUALS(block, 5u);
	}
	
	void testGetFreeIndex(void)
	{
		unsigned long block;
		
		block = 5;
		TS_ASSERT_LESS_THAN(manager->getFreeIndex(block), 0);
		TS_ASSERT_EQUALS(block, 0u);
		
		block = 6;
		TS_ASSERT_LESS_THAN(manager->getFreeIndex(block), 0);
		TS_ASSERT_EQUALS(block, 1u);
		
		block = 7;
		TS_ASSERT_LESS_THAN(manager->getFreeIndex(block), 0);
		TS_ASSERT_EQUALS(block, 2u);
		
		// BlockManager with size 3 -> this should override the first
		// block
		block = 8;
		TS_ASSERT_EQUALS(manager->getFreeIndex(block), 5);
		TS_ASSERT_EQUALS(block, 0u);
		
		block = 7;
		TS_ASSERT_EQUALS(manager->getFreeIndex(block), 6);
		TS_ASSERT_EQUALS(block, 1u);
		
	}
};