/**
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 * SPDX-FileCopyrightText: 2015 Sebastian Rettenberger <rettenbs@in.tum.de>
 */

#include <asagi.h>

// Do not abort to get real failure
#define LOG_ABORT
#include "utils/logger.h"

#include "testdefines.h"

using namespace asagi;

int main(int argc, char** argv) {
  int support;
  MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &support);
  if (support != MPI_THREAD_MULTIPLE)
    logError() << "MPI_THREAD_MULTIPLE not supported by this MPI implementation";

  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  Grid::startCommThread(-1 - rank);

  Grid* grid = Grid::create();
  grid->setComm(MPI_COMM_WORLD);
  grid->setParam("MPI_COMMUNICATION", "THREAD");

  if (grid->open(NC_2D) != Grid::SUCCESS) {
    logError() << "Could not open file";
    return 1;
  }

  int value;

  double coords[2];
  for (int i = 0; i < WIDTH; i++) {
    coords[0] = i;

    for (int j = 0; j < LENGTH; j++) {
      coords[1] = j;

      value = j + i * LENGTH;
      if (grid->getInt(coords) != value) {
        logError() << "Value at" << i << j << "should be" << value << "but is"
                   << grid->getInt(coords);
        return 1;
      }
    }
  }

  unsigned long accesses = grid->getCounter("accesses");
  if (accesses == 0 || accesses > WIDTH * LENGTH * 2) {
    logError() << "Counter \"accesses\" should be less than" << (WIDTH * LENGTH * 2) << "but is"
               << accesses;
    return 1;
  }

  delete grid;

  Grid::stopCommThread();

  MPI_Finalize();

  return 0;
}
