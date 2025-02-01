/**
 * SPDX-License-Identifier: LGPLv3
 *
 * SPDX-FileCopyrightText: 2015 Sebastian Rettenberger <rettenbs@in.tum.de>
 */

#include "globaltest.h"

#include "threads/once.h"

class OnceTest : public CxxTest::TestSuite {
  int executed;
  threads::Once once;

  public:
  void setUp(void) { executed = 0; }

  void testSaveExec(void) {
    once.saveExec(*this, &OnceTest::execute);
    TS_ASSERT_EQUALS(executed, 1);
    once.saveExec(*this, &OnceTest::execute);
    TS_ASSERT_EQUALS(executed, 1);
  }

  private:
  void execute() { executed++; }
};
