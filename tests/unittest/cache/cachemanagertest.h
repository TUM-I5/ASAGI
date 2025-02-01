/**
 * SPDX-License-Identifier: LGPLv3
 *
 * SPDX-FileCopyrightText: 2015 Sebastian Rettenberger <rettenbs@in.tum.de>
 */

#include "globaltest.h"

#include "allocator/default.h"
#include "cache/cachemanager.h"

class CacheManagerTest : public CxxTest::TestSuite {
  unsigned char* cache;
  cache::CacheManager* manager;

  public:
  void setUp(void) {
    cache = new unsigned char[3 * sizeof(int)];
    manager = new cache::CacheManager();
    manager->init(cache, 3, sizeof(int));
  }

  void tearDown(void) {
    delete manager;
    delete[] cache;
  }

  void testGet(void) {
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
