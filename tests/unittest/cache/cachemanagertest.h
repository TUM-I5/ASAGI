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
 * @copyright 2015 Sebastian Rettenberger <rettenbs@in.tum.de>
 */

#include "globaltest.h"

#include "allocator/default.h"
#include "cache/cachemanager.h"

class CacheManagerTest : public CxxTest::TestSuite
{
	unsigned char* cache;
	cache::CacheManager* manager;
public:
	void setUp(void)
	{
		cache = new unsigned char[3*sizeof(int)];
		manager = new cache::CacheManager();
		manager->init(cache, 3, sizeof(int));
	}
	
	void tearDown(void)
	{
		delete manager;
		delete [] cache;
	}
	
	void testGet(void)
	{
		unsigned long cacheId;
		unsigned char* data;
		
		TS_ASSERT_EQUALS(manager->get(5, cacheId, data), -1);
		*reinterpret_cast<int*>(data) = 5;
		manager->unlock(cacheId);

		TS_ASSERT_EQUALS(manager->get(3, cacheId, data), -1);
		*reinterpret_cast<int*>(data) = 3;
		manager->unlock(cacheId);

		TS_ASSERT_EQUALS(manager->get(5, cacheId, data), 5);
		TS_ASSERT_EQUALS(*reinterpret_cast<int*>(data), 5);
		manager->unlock(cacheId);

		TS_ASSERT_EQUALS(manager->get(10, cacheId, data), -1);
		manager->unlock(cacheId);

		TS_ASSERT_EQUALS(manager->get(3, cacheId, data), 3);
		TS_ASSERT_EQUALS(*reinterpret_cast<int*>(data), 3);
		manager->unlock(cacheId);

		// Override first entry
		TS_ASSERT_LESS_THAN_EQUALS(0, manager->get(12, cacheId, data));
		manager->unlock(cacheId);
	}
};
