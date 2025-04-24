/**
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 * SPDX-FileCopyrightText: 2015 Sebastian Rettenberger <rettenbs@in.tum.de>
 */

#ifndef TRANSFER_MPITHREADFULL_H
#define TRANSFER_MPITHREADFULL_H

#ifdef ASAGI_NOMPI
#include "mpino.h"
#else // ASAGI_NOMPI
#include "asagi.h"

#include <cassert>
#include <mutex>

#include "utils/logger.h"

#include "mpi/commthread.h"
#include "mpi/mpicomm.h"
#include "types/type.h"
#endif // ASAGI_NOMPI

/**
 * @brief Transfer classes can copy blocks between MPI/NUMA domains
 */
namespace transfer {

#ifdef ASAGI_NOMPI
/** No MPI transfers with MPI */
typedef MPINo MPIThreadFull;
#else // ASAGI_NOMPI

/**
 * Copies blocks between MPI processes using MPI windows and
 * assuming full storage
 */
class MPIThreadFull : private mpi::Receiver {
  private:
  /** Pointer to the data block */
  const unsigned char* m_data;

  /** Number of elements per block */
  unsigned long m_blockSize;

  /** Size of a single value in bytes */
  unsigned int m_typeSize;

  /** The NUMA domain ID */
  unsigned int m_numaDomainId;

  /** The MPI communicator used by this grid */
  MPI_Comm m_comm;

  /** The type MPI type of an element */
  MPI_Datatype m_mpiType;

  /** The tag used for communication */
  int m_tag;

  /** Lock send-receive pairs to avoid threading issues */
  threads::Mutex* m_sendRecvMutex;

  public:
  MPIThreadFull()
      : m_data(0L), m_blockSize(0), m_typeSize(0), m_numaDomainId(0), m_comm(MPI_COMM_NULL),
        m_mpiType(MPI_DATATYPE_NULL), m_tag(-1), m_sendRecvMutex(0L) {}

  virtual ~MPIThreadFull() {
    if (m_numaDomainId == 0 && m_tag >= 0) {
      mpi::CommThread::commThread.unregisterReceiver(m_tag);

      delete m_sendRecvMutex;
    }
  }

  /**
   * Initialize the transfer class
   *
   * @param data Pointer to the local storage
   * @param blockCount Number local blocks
   * @param blockSize Number of elements in one block
   * @param type The data type of the elements
   * @param mpiComm The MPI communicator
   * @param numaComm The NUMA communicator
   */
  asagi::Grid::Error init(unsigned char* data,
                          unsigned long blockCount,
                          unsigned long blockSize,
                          const types::Type& type,
                          const mpi::MPIComm& mpiComm,
                          numa::NumaComm& numaComm) {
    m_data = data;
    m_blockSize = blockSize;
    m_typeSize = type.size();
    m_numaDomainId = numaComm.domainId();
    m_comm = mpiComm.comm();
    m_mpiType = type.getMPIType();

    if (m_numaDomainId == 0) {
      asagi::Grid::Error err = mpi::CommThread::commThread.registerReceiver(m_comm, *this, m_tag);
      if (err != asagi::Grid::SUCCESS)
        return err;

      m_sendRecvMutex = new threads::Mutex();
    }

    asagi::Grid::Error err = numaComm.broadcast(m_tag);
    if (err != asagi::Grid::SUCCESS)
      return err;

    err = numaComm.broadcast(m_sendRecvMutex);
    if (err != asagi::Grid::SUCCESS)
      return err;

    return asagi::Grid::SUCCESS;
  }

  /**
   * Transfers a block via MPI
   *
   * @param remoteRank Id of the rank that stores the data
   * @param offset Offset of the block on this rank
   * @param cache Pointer to the local cache for this block
   */
  void transfer(int remoteRank, unsigned long offset, unsigned char* cache) {
    int mpiResult;
    NDBG_UNUSED(mpiResult);

    std::lock_guard<threads::Mutex> lock(*m_sendRecvMutex);

    mpi::CommThread::commThread.send(m_tag, remoteRank, offset);

    mpiResult = MPI_Recv(cache, m_blockSize, m_mpiType, remoteRank, 0, m_comm, MPI_STATUS_IGNORE);
    assert(mpiResult == MPI_SUCCESS);
  }

  void recv(int sender, unsigned long blockId) {
    // We get the local blockId

    int mpiResult;
    NDBG_UNUSED(mpiResult);

    mpiResult = MPI_Send(const_cast<unsigned char*>(&m_data[blockId * m_blockSize * m_typeSize]),
                         m_blockSize,
                         m_mpiType,
                         sender,
                         0,
                         m_comm);
    assert(mpiResult == MPI_SUCCESS);
  }
};

#endif // ASAGI_NOMPI

} // namespace transfer

#endif // TRANSFER_MPITHREADFULL_H
