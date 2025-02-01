/**
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 * SPDX-FileCopyrightText: 2012 Sebastian Rettenberger <rettenbs@in.tum.de>
 */

#include <asagi.h>
#include <mpi.h>

#define DEBUG_ABORT MPI_Abort(MPI_COMM_WORLD, 1)
#include "utils/logger.h"

#include "tests.h"

using namespace asagi;

int main(int argc, char** argv) {
  int rank;

  MPI_Init(&argc, &argv);

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  Grid* grid = Grid::create(Grid::FLOAT, Grid::LARGE_GRID);

  if (grid->open(NC_1D) != Grid::SUCCESS)
    return 1;

  for (int i = 0; i < NC_WIDTH; i++)
    if (grid->getInt1D(i) != i) {
      logError() << "Test failed on rank" << rank << std::endl
                 << "Value at" << i << "should be" << i << "but is" << grid->getInt1D(i);
      return 1;
    }

  delete grid;

  MPI_Finalize();

  return 0;
}
