/**
 * SPDX-License-Identifier: LGPLv3
 *
 * SPDX-FileCopyrightText: 2015 Sebastian Rettenberger <rettenbs@in.tum.de>
 */

#include <asagi.h>
#include <pthread.h>

// Do not abort to get real failure
#define LOG_ABORT
#include "utils/logger.h"

#include "testdefines.h"

using namespace asagi;

void* work(void* p);

int main(int argc, char** argv) {
  int provided;
  MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
  if (provided != MPI_THREAD_MULTIPLE) {
    logError() << "Test requires MPI_THREAD_MULTIPLE support";
    return 1;
  }

  Grid* grid = Grid::create();
  grid->setComm(MPI_COMM_WORLD);
  grid->setThreads(2);
  grid->setParam("GRID", "CACHE");

  pthread_t thread;
  pthread_create(&thread, 0L, &work, grid);

  void* ret = work(grid);
  if (ret != 0)
    return reinterpret_cast<std::intptr_t>(ret);

  pthread_join(thread, &ret);
  if (ret != 0)
    return reinterpret_cast<std::intptr_t>(ret);

  unsigned long accesses = grid->getCounter("accesses");
  if (accesses == 0 || accesses > WIDTH * LENGTH * 2) {
    logError() << "Counter \"accesses\" should be less than" << (WIDTH * LENGTH * 2) << "but is"
               << accesses;
    return 1;
  }

#if 0
	// Does not work in these small tests
	if (grid->getCounter("numa_transfers") == 0) {
		logError() << "Counter \"numa_transfers\" should be greater than zero";
		return 1;
	}
#endif

  delete grid;

  MPI_Finalize();

  return 0;
}

void* work(void* p) {
  Grid* grid = static_cast<Grid*>(p);

  if (grid->open(NC_2D) != Grid::SUCCESS) {
    logError() << "Could not open file";
    return reinterpret_cast<void*>(1L);
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
        return reinterpret_cast<void*>(1L);
      }
    }
  }

  return reinterpret_cast<void*>(0L);
}
