/**
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 * SPDX-FileCopyrightText: 2012-2015 Sebastian Rettenberger <rettenbs@in.tum.de>
 */

#include "globaltest.h"
#include "testdefines.h"

#include "grid/grid.h"
#include "grid/simplecontainer.h"
#include "grid/level/passthrough.h"
#include "types/basictype.h"

class PassThroughTest : public CxxTest::TestSuite {
  grid::Grid* c;
  grid::level::PassThrough<types::BasicType<float>>* grid;

  public:
  void setUp(void) {
    // Set up a 1d grid
    c = new grid::Grid(asagi::Grid::FLOAT);
    c->setParam("GRID", "PASS_THROUGH");
    c->open("../../../" NC_1D);
    grid = &dynamic_cast<grid::SimpleContainer<grid::level::PassThrough<types::BasicType<float>>,
                                               types::BasicType<float>>*>(c->m_containers[0])
                ->m_levels[0];

    TS_ASSERT(grid);
  }

  void tearDown(void) { delete c; }

  void testTypeSize(void) { TS_ASSERT_EQUALS(grid->typeSize(), sizeof(float)); }

  void testNumaDomainId(void) { TS_ASSERT_EQUALS(grid->numaDomainId(), grid->numa().domainId()); }

  void testMax(void) { TS_ASSERT_DELTA(grid->max(0), WIDTH - 1 + 0.5, 0.0001); }

  void testDelta(void) { TS_ASSERT_DELTA(grid->delta(0), 1.0, 0.0001); }
};
