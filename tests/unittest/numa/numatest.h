/**
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 * SPDX-FileCopyrightText: 2015 Sebastian Rettenberger <rettenbs@in.tum.de>
 */

#include <pthread.h>

#include "globaltest.h"

#include "numa/numa.h"

class NumaTest : public CxxTest::TestSuite {
  numa::Numa* numa1;
  numa::Numa* numa2;

  unsigned int threadId2;
  bool master2;
  asagi::Grid::Error err2;

  public:
  void setUp(void) {
    numa1 = new numa::Numa;
    numa2 = new numa::Numa;
    numa2->setThreads(2);
  }

  void tearDown(void) { delete numa1; }

  void testRegisterThread(void) {
    bool master;
    TS_ASSERT_EQUALS(numa1->registerThread(master), asagi::Grid::SUCCESS);
    TS_ASSERT(master);

    pthread_t thread;
    pthread_create(&thread, 0L, &NumaTest::thread2_registerThread, this);
    TS_ASSERT_EQUALS(numa2->registerThread(master), asagi::Grid::SUCCESS);

    pthread_join(thread, 0L);
    TS_ASSERT_EQUALS(err2, asagi::Grid::SUCCESS);

    if (numa2->totalDomains() == 1) {
      TS_ASSERT(master ^ master2);
    } else {
      TS_ASSERT(master);
      TS_ASSERT(master2);
    }

    TS_ASSERT_EQUALS(numa2->threadId(), 0u);
    TS_ASSERT_EQUALS(threadId2, 1u);
  }

  void testCreatecComm(void) {
    bool master;
    TS_ASSERT_EQUALS(numa1->registerThread(master), asagi::Grid::SUCCESS);
    TS_ASSERT(master);
    TS_ASSERT(numa1->createComm());

    pthread_t thread;
    pthread_create(&thread, 0L, &NumaTest::thread2_createComm, this);

    TS_ASSERT_EQUALS(numa2->registerThread(master), asagi::Grid::SUCCESS);

    numa::NumaComm* comm = numa2->createComm();
    TS_ASSERT(comm);

    pthread_join(thread, 0L);
    TS_ASSERT_EQUALS(err2, asagi::Grid::SUCCESS);
  }

  private:
  static void* thread2_registerThread(void* ref) {
    NumaTest* ref2 = static_cast<NumaTest*>(ref);
    ref2->err2 = ref2->numa2->registerThread(ref2->master2);
    ref2->threadId2 = ref2->numa2->threadId();
    return 0L;
  }

  static void* thread2_createComm(void* ref) {
    NumaTest* ref2 = static_cast<NumaTest*>(ref);
    ref2->err2 = ref2->numa2->registerThread(ref2->master2);
    if (ref2->err2 != asagi::Grid::SUCCESS)
      return 0L;

    if (ref2->numa2->totalDomains() > 1)
      ref2->numa2->createComm();

    return 0L;
  }
};
