/**
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 * SPDX-FileCopyrightText: 2015 Sebastian Rettenberger <rettenbs@in.tum.de>
 */

#include <cstdint>

#include <asagi.h>

// Do not abort to get real failure
#define LOG_ABORT
#include "utils/logger.h"

#include "testdefines.h"

using namespace asagi;

void* work(void* p);

int main(int argc, char** argv) {
  MPI_Init(&argc, &argv);

  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  Grid* grids[2];

  for (int g = 0; g < 2; g++) {
    grids[g] = Grid::create();
    grids[g]->setComm(MPI_COMM_WORLD);
    grids[g]->setThreads(2);
  }

  pthread_t thread;
  pthread_create(&thread, 0L, &work, grids);

  void* ret = work(grids);
  if (ret != 0)
    return reinterpret_cast<std::intptr_t>(ret);

  pthread_join(thread, &ret);
  if (ret != 0)
    return reinterpret_cast<std::intptr_t>(ret);

  for (int g = 0; g < 2; g++) {
    unsigned long accesses = grids[g]->getCounter("accesses");
    if (accesses == 0 || accesses > WIDTH * LENGTH * 2) {
      logError() << "Counter \"accesses\" should be less than" << (WIDTH * LENGTH * 2) << "but is"
                 << accesses;
      return 1;
    }

#ifdef DEBUG_NUMA
    if (grids[g]->getCounter("numa_transfers") == 0) {
      logError() << "Counter \"numa_transfers\" should be greater than zero";
      return 1;
    }
#endif // DEBUG_NUMA
  }

  for (int g = 0; g < 2; g++)
    delete grids[g];

  MPI_Finalize();

  return 0;
}

void* work(void* p) {
  Grid** grids = static_cast<Grid**>(p);

  for (int g = 0; g < 2; g++) {
    if (grids[g]->open(NC_2D) != Grid::SUCCESS) {
      logError() << "Could not open file";
      return reinterpret_cast<void*>(1L);
    }
  }

  for (int g = 0; g < 2; g++) {
    int value;

    double coords[2];
    for (int i = 0; i < WIDTH; i++) {
      coords[0] = i;

      for (int j = 0; j < LENGTH; j++) {
        coords[1] = j;

        value = j + i * LENGTH;
        if (grids[g]->getInt(coords) != value) {
          logError() << "Value at" << i << j << "should be" << value << "but is"
                     << grids[g]->getInt(coords);
          return reinterpret_cast<void*>(1L);
        }
      }
    }
  }

  return reinterpret_cast<void*>(0L);
}
