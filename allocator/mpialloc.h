/**
 * SPDX-License-Identifier: LGPLv3
 *
 * SPDX-FileCopyrightText: 2013-2015 Sebastian Rettenberger <rettenbs@in.tum.de>
 */

#ifndef ALLOCATOR_MPIALLOC_H
#define ALLOCATOR_MPIALLOC_H

#ifdef ASAGI_NOMPI
#include "default.h"
#else // ASAGI_NOMPI
#include "asagi.h"

#include "mpi/mpicomm.h"
#endif // ASAGI_NOMPI

namespace allocator {

#ifdef ASAGI_NOMPI
/** Use the default allocator if MPI is not available */
typedef Default MPIAlloc;
#else // ASAGI_NOMPI

/**
 * This allocator uses MPI mechanisms to allocate/free memory
 */
class MPIAlloc {
  public:
  /**
   * @copydoc Default::allocate
   */
  template <typename T>
  static asagi::Grid::Error allocate(size_t size, T*& ptr) {
    std::lock_guard<mpi::Lock> lock(mpi::MPIComm::mpiLock);

    if (MPI_Alloc_mem(size * sizeof(T), MPI_INFO_NULL, &ptr) != MPI_SUCCESS)
      return asagi::Grid::MPI_ERROR;

    return asagi::Grid::SUCCESS;
  }

  /**
   * @copydoc Default::free
   */
  template <typename T>
  static void free(T* ptr) {
    if (ptr) {
      std::lock_guard<mpi::Lock> lock(mpi::MPIComm::mpiLock);
      MPI_Free_mem(ptr);
    }
  }
};

#endif // ASAGI_NOMPI

} // namespace allocator

#endif // ALLOCATOR_MPIALLOC_H
