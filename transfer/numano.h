/**
 * SPDX-License-Identifier: LGPLv3
 *
 * SPDX-FileCopyrightText: 2015 Sebastian Rettenberger <rettenbs@in.tum.de>
 */

#ifndef TRANSFER_NUMANO_H
#define TRANSFER_NUMANO_H

namespace transfer {

/**
 * Disables all NUMA copies
 */
class NumaNo {
  private:
  public:
  NumaNo() {}

  virtual ~NumaNo() {}

  /**
   * Initialize the transfer class
   *
   * Stub for {@link NumaFull::init}
   */
  asagi::Grid::Error init(const unsigned char* data,
                          unsigned long blockCount,
                          unsigned long blockSize,
                          const types::Type& type,
                          const mpi::MPIComm& mpiComm,
                          const numa::NumaComm& numaComm,
                          cache::CacheManager& cacheManager) {
    return asagi::Grid::SUCCESS;
  }

  /**
   * Initialize the transfer class
   *
   * Stub for {@link NumaCache::init}
   */
  asagi::Grid::Error init(unsigned long blockSize,
                          const types::Type& type,
                          numa::NumaComm& numaComm,
                          cache::CacheManager& cacheManager) {
    return asagi::Grid::SUCCESS;
  }

  /**
   * Stub for {@link NumaFull::transfer}
   *
   * @return Always false
   */
  bool transfer(unsigned long blockId,
                int remoteRank,
                unsigned int domainId,
                unsigned long offset,
                unsigned char* cache) {
    return false;
  }

  /**
   * Stub for {@link NumaCache::transfer}
   *
   * @return Always false
   */
  bool transfer(unsigned long blockId, unsigned char* cache) { return false; }
};

} // namespace transfer

#endif // TRANSFER_NUMANO_H
