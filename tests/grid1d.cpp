/**
 * SPDX-License-Identifier: LGPLv3
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

  Grid* grid = Grid::create(); // FLOAT is default

  if (grid->open(NC_1D) != Grid::SUCCESS)
    return 1;

  double coords;
  for (int i = 0; i < NC_WIDTH; i++) {
    coords = i;
    if (grid->getInt(&coords) != i) {
      logError() << "Test failed on rank" << rank << std::endl
                 << "Value at" << i << "should be" << i << "but is" << grid->getInt(&coords);
      return 1;
    }
  }

  delete grid;

  MPI_Finalize();

  return 0;
}
