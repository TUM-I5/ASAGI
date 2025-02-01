/**
 * SPDX-License-Identifier: LGPLv3
 *
 * SPDX-FileCopyrightText: 2015 Sebastian Rettenberger <rettenbs@in.tum.de>
 */

#include "globaltest.h"

#include "threads/sync.h"

class SyncTest : public CxxTest::TestSuite {
  threads::Sync sync;

  int value;
  int* pointer;
  bool ret2;

  public:
  void testBarrier(void) {
    pthread_t thread;
    pthread_create(&thread, 0L, &SyncTest::thread2_barrier, this);

    TS_ASSERT(sync.barrier(2));
    TS_ASSERT_EQUALS(value, 42);

    TS_ASSERT(sync.barrier(2));

    pthread_join(thread, 0L);
    TS_ASSERT(ret2);

    TS_ASSERT_EQUALS(value, 5);
  }

  void testWaitBarrier(void) {
    pthread_t thread;
    pthread_create(&thread, 0L, &SyncTest::thread2_waitBarrier, this);

    value = 42;

    TS_ASSERT(sync.startBarrier());
    TS_ASSERT(sync.waitBarrier(2, true));

    TS_ASSERT_EQUALS(value, 0);
    value = 3;

    TS_ASSERT(sync.endBarrier());

    TS_ASSERT_EQUALS(value, 3);

    pthread_join(thread, 0L);
    TS_ASSERT(ret2);
  }

  void testBroadcast(void) {
    pthread_t thread;
    pthread_create(&thread, 0L, &SyncTest::thread2_broadcast, this);

    pointer = 0L;
    int myvalue = 77;
    int* localp = &myvalue;
    TS_ASSERT(sync.broadcast(localp, 2, 0));

    pthread_join(thread, 0L);
    TS_ASSERT(ret2);

    TS_ASSERT(pointer);
    TS_ASSERT_EQUALS(*pointer, 77);
  }

  private:
  static void* thread2_barrier(void* ref) {
    SyncTest* ref2 = static_cast<SyncTest*>(ref);

    ref2->value = 42;
    ref2->ret2 = ref2->sync.barrier(2);
    if (!ref2->ret2)
      return 0L;

    ref2->ret2 = ref2->sync.barrier(2);
    if (!ref2->ret2)
      return 0L;

    ref2->value = 5;

    return 0L;
  }

  static void* thread2_waitBarrier(void* ref) {
    SyncTest* ref2 = static_cast<SyncTest*>(ref);

    ref2->ret2 = ref2->sync.startBarrier();
    if (!ref2->ret2)
      return 0L;

    ref2->ret2 = ref2->sync.waitBarrier(2);
    if (!ref2->ret2)
      return 0L;

    ref2->value = 0;

    ref2->ret2 = ref2->sync.endBarrier();
    if (!ref2->ret2)
      return 0L;

    return 0L;
  }

  static void* thread2_broadcast(void* ref) {
    SyncTest* ref2 = static_cast<SyncTest*>(ref);

    ref2->ret2 = ref2->sync.broadcast(ref2->pointer, 2, 1);
    if (!ref2->ret2)
      return 0L;

    return 0L;
  }
};
