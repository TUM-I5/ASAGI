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
#ifndef ASAGI_NOMPI
  MPI_Init(&argc, &argv);
#endif // ASAGI_NOMPI

  Grid* grid = Grid::createArray();

  if (grid->open(NC_2DCOMPOUND) != Grid::SUCCESS) {
    logError() << "Could not open file";
    return 1;
  }

  if (grid->getVarSize() != 2 * sizeof(float)) {
    logError() << "Wrong variable size";
    return 1;
  }

  int value;

  double coords[2];
  for (int i = 0; i < WIDTH; i++) {
    coords[0] = i;

    for (int j = 0; j < LENGTH; j++) {
      coords[1] = j;

      value = j + i * LENGTH;

      float buf[2];
      grid->getBuf(buf, coords);

      if (buf[0] != value) {
        logError() << "Value 0 at" << i << j << "should be" << value << "but is" << buf[0];
        return 1;
      }
      if (buf[1] != value + 10000) { // TODO make the addition configurable
        logError() << "Value 1 at" << i << j << "should be" << value + 10000 << "but is" << buf[1];
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
