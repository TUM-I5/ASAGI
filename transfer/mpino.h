/**
 * SPDX-License-Identifier: LGPLv3
 *
 * SPDX-FileCopyrightText: 2015 Sebastian Rettenberger <rettenbs@in.tum.de>
 */

#ifndef TRANSFER_MPINO_H
#define TRANSFER_MPINO_H

#include <cassert>

namespace transfer {

/**
 * Disables MPI transfers
 */
class MPINo {
  public:
  MPINo() {}

  virtual ~MPINo() {}

  /**
   * Initialize the transfer class
   *
   * Stub for {@link MPIWinFull::init} and {@link MPIThreadFull::init}
   */
  asagi::Grid::Error init(unsigned char* data,
                          unsigned long blockCount,
                          unsigned long blockSize,
                          const types::Type& type,
                          const mpi::MPIComm& mpiComm,
                          const numa::NumaComm& numaComm) {
    return asagi::Grid::SUCCESS;
  }

  /**
   * Initialize the transfer class
   *
   * Stub for {@link MPICache::init}
   */
  asagi::Grid::Error init(unsigned char* cache,
                          unsigned int cacheSize,
                          const cache::CacheManager& cachemManager,
                          unsigned long blockCount,
                          unsigned long blockSize,
                          const types::Type& type,
                          const mpi::MPIComm& mpiComm,
                          const numa::NumaComm& numaComm) {
    return asagi::Grid::SUCCESS;
  }

  /**
   * Should never be called
   *
   * Stub for {@link MPIWinFull::transfer} and {@link MPIThreadFull::transfer}
   */
  void transfer(int remoteRank, unsigned long offset, unsigned char* cache) { assert(false); }

  /**
   * Starts the transfer of a new block.
   *
   * Stub for {@link MPIWinCache::startTransfer} and
   * {@link MPIThreadCache::startTransfer}
   */
  long startTransfer(unsigned long blockId,
                     int dictRank,
                     unsigned long dictOffset,
                     unsigned long offset) {
    return -1;
  }

  /**
   * Will not transfer anything
   *
   * Stub for {@link MPIWinCache::transfer} and
   * {@link MPIThreadCache::transfer}
   */
  bool transfer(long entry, unsigned long blockId, unsigned char* cache, bool& retry) {
    retry = false;
    return false;
  }

  /**
   * Ends a transfer phase started with {@link startTransfer}
   *
   * Stub for {@link MPIWinCache::endTransfer} and
   * {@link MPIThreadCache::endTransfer}
   */
  void endTransfer(unsigned long blockId,
                   int dictRank,
                   unsigned long dictOffset,
                   unsigned long offset) {}

  /**
   * Adds information about a local stored block
   *
   * Stub for {@link MPIWinCache::addBlock} and
   * {@link MPIThreadCache::addBlock}
   */
  void addBlock(unsigned long blockId,
                int dictRank,
                unsigned long dictOffset,
                unsigned long offset) {}

  /**
   * Deletes information about a local stored block
   *
   * Stub for {@link MPIWinCache::deleteBlock} and
   * {@link MPIThreadCache::deleteBlock}
   */
  void deleteBlock(long blockId, int dictRank, unsigned long dictOffset, unsigned long offset) {}
};

} // namespace transfer

#endif // TRANSFER_MPINO_H
