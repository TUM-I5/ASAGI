/**
 * SPDX-License-Identifier: LGPLv3
 *
 * SPDX-FileCopyrightText: 2012-2015 Sebastian Rettenberger <rettenbs@in.tum.de>
 */

#include "globaltest.h"
#include "testdefines.h"

#include <algorithm>

#undef NDEBUG

#include "io/netcdfreader.h"

class NetCdfReaderTest : public CxxTest::TestSuite {
  io::NetCdfReader* file3d;

  public:
  void setUp(void) {
    file3d = new io::NetCdfReader("../../" NC_3D, 0);
    TS_ASSERT_EQUALS(file3d->open(), asagi::Grid::SUCCESS);
  }

  void tearDown(void) { delete file3d; }

  void testGetDimensions(void) { TS_ASSERT_EQUALS(file3d->dimensions(), 3u); }

  void testGetSize(void) {
    TS_ASSERT_EQUALS(file3d->getSize(0), static_cast<unsigned long>(HEIGHT));
    TS_ASSERT_EQUALS(file3d->getSize(1), static_cast<unsigned long>(LENGTH));
    TS_ASSERT_EQUALS(file3d->getSize(2), static_cast<unsigned long>(WIDTH));
  }

  /**
   * @todo This test only tests 3d grids at the moment
   */
  void testGetBlock(void) {
    float values[10][10][10];

    size_t offsets[3] = {0, 0, 0};
    size_t sizes[3] = {10, 10, 10};
    TS_ASSERT_THROWS_NOTHING(file3d->getBlock<float>(values, offsets, sizes));
    for (unsigned int i = 0; i < std::min(HEIGHT - offsets[0], sizes[0]); i++)
      for (unsigned int j = 0; j < std::min(LENGTH - offsets[1], sizes[1]); j++)
        for (unsigned int k = 0; k < std::min(WIDTH - offsets[2], sizes[2]); k++)
          TS_ASSERT_EQUALS(values[i][j][k], calcValueAt(i, j, k, offsets));

    size_t offsets2[3] = {9, 4, 63};
    TS_ASSERT_THROWS_NOTHING(file3d->getBlock<float>(values, offsets2, sizes));
    for (unsigned int i = 0; i < std::min(HEIGHT - offsets2[0], sizes[0]); i++)
      for (unsigned int j = 0; j < std::min(LENGTH - offsets2[1], sizes[1]); j++)
        for (unsigned int k = 0; k < std::min(WIDTH - offsets2[2], sizes[2]); k++)
          TS_ASSERT_EQUALS(values[i][j][k], calcValueAt(i, j, k, offsets2));

    size_t offsets3[3] = {0, 62, 21};
    TS_ASSERT_THROWS_NOTHING(file3d->getBlock<float>(values, offsets3, sizes));
    for (unsigned int i = 0; i < std::min(HEIGHT - offsets3[0], sizes[0]); i++)
      for (unsigned int j = 0; j < std::min(LENGTH - offsets3[1], sizes[1]); j++)
        for (unsigned int k = 0; k < std::min(WIDTH - offsets3[2], sizes[2]); k++)
          TS_ASSERT_EQUALS(values[i][j][k], calcValueAt(i, j, k, offsets3));

    size_t offsets4[3] = {65, 10, 0};
    TS_ASSERT_THROWS_NOTHING(file3d->getBlock<float>(values, offsets4, sizes));
    for (unsigned int i = 0; i < std::min(HEIGHT - offsets4[0], sizes[0]); i++)
      for (unsigned int j = 0; j < std::min(LENGTH - offsets4[1], sizes[1]); j++)
        for (unsigned int k = 0; k < std::min(WIDTH - offsets4[2], sizes[2]); k++)
          TS_ASSERT_EQUALS(values[i][j][k], calcValueAt(i, j, k, offsets4));

    float values2[64][64][64];
    size_t offsets5[3] = {55, 55, 0};
    size_t sizes2[3] = {64, 64, 64};
    TS_ASSERT_THROWS_NOTHING(file3d->getBlock<float>(values2, offsets5, sizes2));
    for (unsigned int i = 0; i < std::min(HEIGHT - offsets5[0], sizes2[0]); i++)
      for (unsigned int j = 0; j < std::min(LENGTH - offsets5[1], sizes2[1]); j++)
        for (unsigned int k = 0; k < std::min(WIDTH - offsets5[2], sizes2[2]); k++)
          TS_ASSERT_EQUALS(values2[i][j][k], calcValueAt(i, j, k, offsets5));

    // Test float to double conversion
    double values3[10][10][10];
    TS_ASSERT_THROWS_NOTHING(file3d->getBlock<double>(values3, offsets3, sizes));
    for (unsigned int i = 0; i < std::min(HEIGHT - offsets3[0], sizes[0]); i++)
      for (unsigned int j = 0; j < std::min(LENGTH - offsets3[1], sizes[1]); j++)
        for (unsigned int k = 0; k < std::min(WIDTH - offsets3[2], sizes[2]); k++)
          TS_ASSERT_EQUALS(values3[i][j][k], calcValueAt(i, j, k, offsets3));

    // Test void
    TS_ASSERT_THROWS_NOTHING(file3d->getBlock<void>(values2, offsets5, sizes2));
    for (unsigned int i = 0; i < std::min(HEIGHT - offsets5[0], sizes2[0]); i++)
      for (unsigned int j = 0; j < std::min(LENGTH - offsets5[1], sizes2[1]); j++)
        for (unsigned int k = 0; k < std::min(WIDTH - offsets5[2], sizes2[2]); k++)
          TS_ASSERT_EQUALS(values2[i][j][k], calcValueAt(i, j, k, offsets5));
  }

  private:
  /**
   * @return The value that should be in the netcdf file at x, y, z
   */
  float calcValueAt(int z, int y, int x, size_t offsets[3]) {
    return ((x + offsets[2]) * LENGTH + (y + offsets[1])) * WIDTH + (z + offsets[0]);
  }
};
