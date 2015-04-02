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
#include "tests.h"

#include "cache/cachemanager.h"


class ArrayTypeTest : public CxxTest::TestSuite
{
	cache::CacheManager* manager;
public:
	void setUp(void)
	{
		manager = new cache::CacheManager();
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
		TS_ASSERT(!manager->getIndex(block, block));
		TS_ASSERT_EQUALS(block, 5u);
		
		manager->getFreeIndex(block, block);
		block = 6;
		manager->getFreeIndex(block, block);
		block = 7;
		manager->getFreeIndex(block, block);
		
		block = 6;
		TS_ASSERT(manager->getIndex(block, block));
		TS_ASSERT_EQUALS(block, 1u);
		
		// Override first entry
		block = 8;
		manager->getFreeIndex(block, block);
		
		block = 8;
		TS_ASSERT(manager->getIndex(block, block));
		TS_ASSERT_EQUALS(block, 0u);
		
		block = 5;
		TS_ASSERT(!manager->getIndex(block, block));
		TS_ASSERT_EQUALS(block, 5u);
	}
	
	void testGetFreeIndex(void)
	{
		unsigned long block;
		
		block = 5;
		TS_ASSERT_LESS_THAN(manager->getFreeIndex(block, block), 0);
		TS_ASSERT_EQUALS(block, 0u);
		
		block = 6;
		TS_ASSERT_LESS_THAN(manager->getFreeIndex(block, block), 0);
		TS_ASSERT_EQUALS(block, 1u);
		
		block = 7;
		TS_ASSERT_LESS_THAN(manager->getFreeIndex(block, block), 0);
		TS_ASSERT_EQUALS(block, 2u);
		
		// BlockManager with size 3 -> this should override the first
		// block
		block = 8;
		TS_ASSERT_EQUALS(manager->getFreeIndex(block, block), 5);
		TS_ASSERT_EQUALS(block, 0u);
		
		block = 7;
		TS_ASSERT_EQUALS(manager->getFreeIndex(block, block), 6);
		TS_ASSERT_EQUALS(block, 1u);
	}
};
