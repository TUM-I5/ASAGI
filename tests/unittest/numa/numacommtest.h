/**
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 * SPDX-FileCopyrightText: 2015 Sebastian Rettenberger <rettenbs@in.tum.de>
 */

#include <pthread.h>

#include "globaltest.h"

#include "allocator/default.h"
#include "numa/numacomm.h"

class NumaCommTest : public CxxTest::TestSuite {
  numa::Numa* numa;
  numa::NumaComm* numacomm;

  int* data2_1;
  int* data2_2;
  asagi::Grid::Error err2;

  public:
  void setUp(void) {
    // Create to NUMA domains;
    numa = new numa::Numa();
    numa->setThreads(2);

    numacomm = new numa::NumaComm(*numa);
  }

  void tearDown(void) {
    delete numacomm;
    delete numa;
  }

  void testBroadcast(void) {
    pthread_t thread;
    pthread_create(&thread, 0L, &NumaCommTest::thread2_broadcast, this);

    bool master;
    TS_ASSERT_EQUALS(numa->registerThread(master), asagi::Grid::SUCCESS);

    int value;
    int* data = &value;

    // First broadcast
    TS_ASSERT_EQUALS(numacomm->broadcast(data, numacomm->domainId()), asagi::Grid::SUCCESS);

    // Second broadcast
    TS_ASSERT_EQUALS(numacomm->broadcast(data, numacomm->domainId()), asagi::Grid::SUCCESS);

    pthread_join(thread, 0L);
    TS_ASSERT_EQUALS(err2, asagi::Grid::SUCCESS);

    if (numacomm->totalDomains() > 1) {
      TS_ASSERT_EQUALS(data2_1, data);
      TS_ASSERT_EQUALS(data2_2, data);
    }
  }

  void testAllocate(void) {
    pthread_t thread;
    pthread_create(&thread, 0L, &NumaCommTest::thread2_allocate, this);

    bool master;
    TS_ASSERT_EQUALS(numa->registerThread(master), asagi::Grid::SUCCESS);

    int* data = 0L;

    TS_ASSERT_EQUALS(numacomm->allocate<allocator::Default>(1, data), asagi::Grid::SUCCESS);

    pthread_join(thread, 0L);
    TS_ASSERT_EQUALS(err2, asagi::Grid::SUCCESS);

    if (numacomm->totalDomains() == 1) {
      TS_ASSERT(data);
    } else {
      TS_ASSERT_EQUALS(data + 1, data2_1);
    }
  }

  private:
  static void* thread2_broadcast(void* ref) {
    NumaCommTest* ref2 = static_cast<NumaCommTest*>(ref);

    bool master;
    ref2->err2 = ref2->numa->registerThread(master);
    if (ref2->err2 != asagi::Grid::SUCCESS)
      return 0L;

    if (ref2->numacomm->totalDomains() == 1) {
      // Some domain -> only one can allocate the memory
      ref2->data2_1 = 0L;
      ref2->data2_2 = 0L;
    } else {
      ref2->err2 = ref2->numacomm->broadcast(ref2->data2_1, 1 - ref2->numacomm->domainId());
      if (ref2->err2 != asagi::Grid::SUCCESS)
        return 0L;
      ref2->err2 = ref2->numacomm->broadcast(ref2->data2_2, 1 - ref2->numacomm->domainId());
    }

    return 0L;
  }

  static void* thread2_allocate(void* ref) {
    NumaCommTest* ref2 = static_cast<NumaCommTest*>(ref);

    bool master;
    ref2->err2 = ref2->numa->registerThread(master);
    if (ref2->err2 != asagi::Grid::SUCCESS)
      return 0L;

    if (ref2->numacomm->totalDomains() == 1)
      // Some domain -> only one can allocate the memory
      ref2->data2_1 = 0L;
    else
      ref2->err2 = ref2->numacomm->allocate<allocator::Default>(1, ref2->data2_1);

    return 0L;
  }
};
