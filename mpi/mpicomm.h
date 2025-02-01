/**
 * SPDX-License-Identifier: LGPLv3
 *
 * SPDX-FileCopyrightText: 2015 Sebastian Rettenberger <rettenbs@in.tum.de>
 */

#ifndef MPI_MPICOMM_H
#define MPI_MPICOMM_H

#ifdef ASAGI_NOMPI
#include "nompicomm.h"
#else // ASAGI_NOMPI
#include "asagi.h"

#include <mutex>
#endif // ASAGI_NOMPI

#ifdef THREADSAFE_MPI
#include "threads/mutex.h"
#else // THREADSAFE_MPI
#include "threads/noopmutex.h"
#endif // THREADSAFE_MPI

namespace mpi {

/** Type of the global MPI lock */
#ifdef THREADSAFE_MPI
typedef threads::Mutex Lock;
#else  // THREADSAFE_MPI
typedef threads::NoopMutex Lock;
#endif // THREADSAFE_MPI

#ifdef ASAGI_NOMPI
/** MPI Communicator replacement if MPI is disabled */
typedef NoMPIComm MPIComm;
#else // ASAGI_NOMPI

/**
 * Small wrapper around the MPI communicator
 */
class MPIComm {
  private:
  /** The communicator */
  MPI_Comm m_comm;

  /** My rank */
  int m_rank;

  /** Total number of ranks */
  int m_size;

  /** MPI key for next free tag */
  int m_nextFreeTag;

  public:
  MPIComm() : m_comm(MPI_COMM_SELF), m_rank(0), m_size(1), m_nextFreeTag(0) {}

  virtual ~MPIComm() {
    std::lock_guard<Lock> lock(mpiLock);
    freeComm();
  }

  /**
   * Initialize the communicator
   */
  asagi::Grid::Error init(MPI_Comm comm) {
    std::lock_guard<Lock> lock(mpiLock);

    freeComm();

    if (MPI_Comm_dup(comm, &m_comm) != MPI_SUCCESS)
      return asagi::Grid::MPI_ERROR;

    if (MPI_Comm_rank(m_comm, &m_rank) != MPI_SUCCESS)
      return asagi::Grid::MPI_ERROR;
    if (MPI_Comm_size(m_comm, &m_size) != MPI_SUCCESS)
      return asagi::Grid::MPI_ERROR;

    return asagi::Grid::SUCCESS;
  }

  /**
   * @return The associated communicator
   */
  MPI_Comm comm() const { return m_comm; }

  /**
   * @return My rank
   */
  int rank() const { return m_rank; }

  /**
   * @return Size of the communicator
   */
  int size() const { return m_size; }

  /**
   * MPI barrier
   */
  void barrier() const { MPI_Barrier(m_comm); }

  /**
   * Reserves <code>num</code> consecutive tags in this communicator
   *
   * @param num The number of tags that should be reserved
   * @return The first tag that was reserved
   */
  int reserveTags(unsigned int num) {
    int tag = m_nextFreeTag;
    m_nextFreeTag += num;
    return tag;
  }

  private:
  /**
   * Free the current communicator
   *
   * @warning This function does not lock {@link mpiLock}
   */
  int freeComm() {
    if (m_comm != MPI_COMM_SELF)
      return MPI_Comm_free(&m_comm);
    return MPI_SUCCESS;
  }

  public:
  static Lock mpiLock;
};

#endif // ASAGI_NOMPI

} // namespace mpi

#endif // MPI_MPICOMM_H
