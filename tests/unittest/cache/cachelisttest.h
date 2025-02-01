/**
 * SPDX-License-Identifier: LGPLv3
 *
 * SPDX-FileCopyrightText: 2012-2015 Sebastian Rettenberger <rettenbs@in.tum.de>
 */

#include "globaltest.h"

#include "cache/cachelist.h"

class CacheListTest : public CxxTest::TestSuite {
  cache::CacheList* list;

  public:
  void setUp(void) {
    list = new cache::CacheList();
    list->init(3);
  }

  void tearDown(void) { delete list; }

  void testGetIndex(void) {
    unsigned long block;

    block = 5;
    TS_ASSERT(!list->getIndex(block, block));
    TS_ASSERT_EQUALS(block, 5u);

    list->getFreeIndex(block, block);
    block = 6;
    list->getFreeIndex(block, block);
    block = 7;
    list->getFreeIndex(block, block);

    block = 6;
    TS_ASSERT(list->getIndex(block, block));
    TS_ASSERT_EQUALS(block, 1u);

    // Override first entry
    block = 8;
    list->getFreeIndex(block, block);

    block = 8;
    TS_ASSERT(list->getIndex(block, block));
    TS_ASSERT_EQUALS(block, 0u);

    block = 5;
    TS_ASSERT(!list->getIndex(block, block));
    TS_ASSERT_EQUALS(block, 5u);
  }

  void testGetFreeIndex(void) {
    unsigned long block;

    block = 5;
    TS_ASSERT_LESS_THAN(list->getFreeIndex(block, block), 0);
    TS_ASSERT_EQUALS(block, 0u);

    block = 6;
    TS_ASSERT_LESS_THAN(list->getFreeIndex(block, block), 0);
    TS_ASSERT_EQUALS(block, 1u);

    block = 7;
    TS_ASSERT_LESS_THAN(list->getFreeIndex(block, block), 0);
    TS_ASSERT_EQUALS(block, 2u);

    // BlockManager with size 3 -> this should override the first
    // block
    block = 8;
    TS_ASSERT_EQUALS(list->getFreeIndex(block, block), 5);
    TS_ASSERT_EQUALS(block, 0u);

    block = 7;
    TS_ASSERT_EQUALS(list->getFreeIndex(block, block), 6);
    TS_ASSERT_EQUALS(block, 1u);
  }
};
