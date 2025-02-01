/**
 * SPDX-License-Identifier: LGPLv3
 *
 * SPDX-FileCopyrightText: 2015 Sebastian Rettenberger <rettenbs@in.tum.de>
 */

#ifndef TRANSFER_NUMAFULLCACHE_H
#define TRANSFER_NUMAFULLCACHE_H

#include <cstring>

#include "numafull.h"
#include "numacache.h"
#include "cache/cachemanager.h"
#include "threads/mutex.h"

namespace transfer {

/**
 * Copies blocks between NUMA assuming full storage.
 * Will also copy blocks from other caches.
 */
class NumaFullCache : public NumaFull, public NumaCache {
  public:
  NumaFullCache() {}

  virtual ~NumaFullCache() {}

  /**
   * @copydoc NumaFull::init
   */
  asagi::Grid::Error init(const unsigned char* data,
                          unsigned long blockCount,
                          unsigned long blockSize,
                          const types::Type& type,
                          const mpi::MPIComm& mpiComm,
                          numa::NumaComm& numaComm,
                          cache::CacheManager& cacheManager) {
    asagi::Grid::Error err =
        NumaFull::init(data, blockCount, blockSize, type, mpiComm, numaComm, cacheManager);
    if (err != asagi::Grid::SUCCESS)
      return err;

    err = NumaCache::init(blockSize, type, numaComm, cacheManager);
    if (err != asagi::Grid::SUCCESS)
      return err;

    return asagi::Grid::SUCCESS;
  }

  /**
   * @copydoc NumaFill:transfer
   * Will also lock into other NUMA caches
   */
  bool transfer(unsigned long blockId,
                int remoteRank,
                unsigned int domainId,
                unsigned long offset,
                unsigned char* cache) {
    if (NumaFull::transfer(blockId, remoteRank, domainId, offset, cache))
      return true;

    return NumaCache::transfer(blockId, cache);
  }
};

} // namespace transfer

#endif // TRANSFER_NUMAFULLCACHE_H
