/**
 * SPDX-License-Identifier: LGPL-3.0-or-later
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
  Grid* grid = Grid::create();

  if (grid->open(NC_3D) != Grid::SUCCESS) {
    logError() << "Could not open file";
    return 1;
  }

  int value;

  double coords[3];
  for (int i = 0; i < WIDTH; i++) {
    coords[0] = i;

    for (int j = 0; j < LENGTH; j++) {
      coords[1] = j;

      for (int k = 0; k < HEIGHT; k++) {
        coords[2] = k;

        value = k + HEIGHT * (j + i * LENGTH);
        if (grid->getInt(coords) != value) {
          logError() << "Value at" << i << j << "should be" << value << "but is"
                     << grid->getInt(coords);
          return 1;
        }
      }
    }
  }

  if (grid->getCounter("accesses") != WIDTH * LENGTH * HEIGHT) {
    logError() << "Counter \"accesses\" should be" << (WIDTH * LENGTH * HEIGHT) << "but is"
               << grid->getCounter("accesses");
    return 1;
  }

  if (grid->getCounter("local_hits") != WIDTH * LENGTH * HEIGHT) {
    logError() << "Counter \"file_loads\" should be" << (WIDTH * LENGTH * HEIGHT) << "but is"
               << grid->getCounter("file_loads");
    return 1;
  }

  delete grid;

  return 0;
}
