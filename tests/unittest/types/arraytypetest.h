/**
 * SPDX-License-Identifier: LGPLv3
 *
 * SPDX-FileCopyrightText: 2012 Sebastian Rettenberger <rettenbs@in.tum.de>
 */

#include "globaltest.h"
#include "testdefines.h"

#include "types/arraytype.h"

class ArrayTypeTest : public CxxTest::TestSuite {
  io::NetCdfReader* file;

  public:
  void setUp(void) {
    file = new io::NetCdfReader("../../" NC_1D, 0);
    file->open();
  }

  void tearDown(void) { delete file; }

  void testGetSize(void) {
    types::ArrayType<float> type;
    type.check(*file);

    TS_ASSERT_EQUALS(type.size(), sizeof(float));
  }
};
