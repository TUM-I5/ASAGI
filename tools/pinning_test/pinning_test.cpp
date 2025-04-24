/**
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 * SPDX-FileCopyrightText: 2015 Sebastian Rettenberger <rettenbs@in.tum.de>
 */

#include <mpi.h>

#include <omp.h>
#include <iostream>
#include <sstream>
#include <string>
#include <pthread.h>

int main(int argc, char* argv[]) {
  MPI_Init(&argc, &argv);

  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

#pragma omp parallel
  {
    cpu_set_t mask;
    pthread_getaffinity_np(pthread_self(), sizeof(cpu_set_t), &mask);

    int cpus = CPU_COUNT(&mask);

    std::ostringstream ss;
    ss << "Found " << cpus << " CPUs on rank " << rank << ", thread " << omp_get_thread_num()
       << ": ";
    int out = 0, current = 0;
    while (out < cpus) {
      if (CPU_ISSET(current, &mask)) {
        ss << current << ", ";
        out++;
      }
      current++;
    }

    std::string outString = ss.str();
    outString = outString.substr(0, outString.size() - 2);
    std::cout << outString << std::endl;
  }

  MPI_Finalize();

  return 0;
}
