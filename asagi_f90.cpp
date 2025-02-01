/**
 * SPDX-License-Identifier: LGPLv3
 *
 * SPDX-FileCopyrightText: 2012-2015 Sebastian Rettenberger <rettenbs@in.tum.de>
 */

#include "asagi_f90.h"
#include "grid/grid.h"

// Init functions

/**
 * This function is part of the C++ <-> Fortran interface
 */
int f90asagi_grid_create(asagi_type type) {
  return static_cast<grid::Grid*>(asagi::Grid::create(type))->c2f();
}

/**
 * This function is part of the C++ <-> Fortran interface
 */
int f90asagi_grid_create_array(asagi_type basic_type) {
  return static_cast<grid::Grid*>(asagi::Grid::createArray(basic_type))->c2f();
}

/**
 * This function is part of the C++ <-> Fortran interface
 */
int f90asagi_grid_create_struct(int count,
                                int blockLength[],
                                long displacements[],
                                asagi_type types[]) {
  // Not sure of reinterpret_cast is save here
  return static_cast<grid::Grid*>(
             asagi::Grid::createStruct(count,
                                       reinterpret_cast<unsigned int*>(blockLength),
                                       reinterpret_cast<unsigned long*>(displacements),
                                       types))
      ->c2f();
}

/**
 * This function is part of the C++ <-> Fortran interface
 */
void f90asagi_grid_set_comm(int grid_id, int comm) {
  // This function is a no-op if called on a version without MPI support
#ifndef ASAGI_NOMPI
  // MPI_Comm_f2c expects an MPI_Fint, however iso_c_bindings
  // already converts this parameter into c integer
  grid::Grid::f2c(grid_id)->setComm(MPI_Comm_f2c(comm));
#endif // ASAGI_NOMPI
}

/**
 * This function is part of the C++ <-> Fortran interface
 */
void f90asagi_grid_set_threads(int grid_id, int threads) {
  grid::Grid::f2c(grid_id)->setThreads(threads);
}

/**
 * This function is part of the C++ <-> Fortran interface
 */
void f90asagi_grid_set_param(int grid_id, const char* name, const char* value, int level) {
  grid::Grid::f2c(grid_id)->setParam(name, value, level);
}

/**
 * This function is part of the C++ <-> Fortran interface
 */
asagi_error f90asagi_grid_open(int grid_id, const char* filename, int level) {
  return grid::Grid::f2c(grid_id)->open(filename, level);
}

/**
 * This function is part of the C++ <-> Fortran interface
 */
int f90asagi_grid_dimensions(int grid_id) { return grid::Grid::f2c(grid_id)->getDimensions(); }

// Min/Max functions

/**
 * This function is part of the C++ <-> Fortran interface
 */
double f90asagi_grid_min(int grid_id, int n) { return grid::Grid::f2c(grid_id)->getMin(n); }
/**
 * This function is part of the C++ <-> Fortran interface
 */
double f90asagi_grid_max(int grid_id, int n) { return grid::Grid::f2c(grid_id)->getMax(n); }

/**
 * This function is part of the C++ <-> Fortran interface
 */
double f90asagi_grid_delta(int grid_id, int n, int level) {
  return grid::Grid::f2c(grid_id)->getDelta(n, level);
}

/**
 * This function is part of the C++ <-> Fortran interface
 */
int f90asagi_grid_var_size(int grid_id) { return grid::Grid::f2c(grid_id)->getVarSize(); }

// Getters

/**
 * This function is part of the C++ <-> Fortran interface
 */
unsigned char f90asagi_grid_get_byte(int grid_id, double* pos, int level) {
  return grid::Grid::f2c(grid_id)->getByte(pos, level);
}
/**
 * This function is part of the C++ <-> Fortran interface
 */
int f90asagi_grid_get_int(int grid_id, double* pos, int level) {
  return grid::Grid::f2c(grid_id)->getInt(pos, level);
}
/**
 * This function is part of the C++ <-> Fortran interface
 */
long f90asagi_grid_get_long(int grid_id, double* pos, int level) {
  return grid::Grid::f2c(grid_id)->getLong(pos, level);
}
/**
 * This function is part of the C++ <-> Fortran interface
 */
float f90asagi_grid_get_float(int grid_id, double* pos, int level) {
  return grid::Grid::f2c(grid_id)->getFloat(pos, level);
}
/**
 * This function is part of the C++ <-> Fortran interface
 */
double f90asagi_grid_get_double(int grid_id, double* pos, int level) {
  return grid::Grid::f2c(grid_id)->getDouble(pos, level);
}
/**
 * This function is part of the C++ <-> Fortran interface
 */
void f90asagi_grid_get_buf(int grid_id, void* buf, double* pos, int level) {
  grid::Grid::f2c(grid_id)->getBuf(buf, pos, level);
}

// destructor

/**
 * This function is part of the C++ <-> Fortran interface
 */
void f90asagi_grid_close(int grid_id) { delete grid::Grid::f2c(grid_id); }

// Communication thread

/**
 * This function is part of the C++ <-> Fortran interface
 */
asagi_error f90asagi_start_comm_thread(int sched_cpu, int comm) {
  // This function is a no-op if called on a version without MPI support
#ifndef ASAGI_NOMPI
  return grid::Grid::startCommThread(sched_cpu, MPI_Comm_f2c(comm));
#endif // ASAGI_NOMPI
  return asagi::Grid::SUCCESS;
}

/**
 * This function is part of the C++ <-> Fortran interface
 */
void f90asagi_stop_comm_thread() {
  // This function is a no-op if called on a version without MPI support
#ifndef ASAGI_NOMPI
  grid::Grid::stopCommThread();
#endif // ASAGI_NOMPI
}

/**
 * This function is part of the C++ <-> Fortran interface
 */
int f90asagi_node_local_rank(int comm) {
#ifndef ASAGI_NOMPI
  return grid::Grid::nodeLocalRank(MPI_Comm_f2c(comm));
#endif // ASAGI_NOMPI
  return 0;
}
