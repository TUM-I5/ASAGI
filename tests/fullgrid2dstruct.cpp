/**
 * SPDX-License-Identifier: LGPLv3
 *
 * SPDX-FileCopyrightText: 2012-2015 Sebastian Rettenberger <rettenbs@in.tum.de>
 */

#include <asagi.h>

// Do not abort to get real failure
#define LOG_ABORT
#include "utils/logger.h"

#include "testdefines.h"

using namespace asagi;

int main(int argc, char** argv) {
#ifndef ASAGI_NOMPI
  MPI_Init(&argc, &argv);
#endif // ASAGI_NOMPI

  // TODO maybe we should create a real struct in netCDF
  unsigned int blockSize = 1;
  unsigned long displacement = 0;
  asagi::Grid::Type type = asagi::Grid::FLOAT;
  Grid* grid = Grid::createStruct(1, &blockSize, &displacement, &type);

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
      if (grid->getFloat(coords) != value) {
        logError() << "Value at" << i << j << "should be" << value << "but is"
                   << grid->getInt(coords);
        return 1;
      }
    }
  }

  if (grid->getCounter("accesses") != WIDTH * LENGTH) {
    logError() << "Counter \"accesses\" should be" << (WIDTH * LENGTH) << "but is"
               << grid->getCounter("accesses");
    return 1;
  }

  if (grid->getCounter("local_hits") != WIDTH * LENGTH) {
    logError() << "Counter \"file_loads\" should be" << (WIDTH * LENGTH) << "but is"
               << grid->getCounter("file_loads");
    return 1;
  }

  delete grid;

#ifndef ASAGI_NOMPI
  MPI_Finalize();
#endif // ASAGI_NOMPI

  return 0;
}
