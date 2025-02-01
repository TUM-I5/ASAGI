/**
 * SPDX-License-Identifier: LGPLv3
 *
 * SPDX-FileCopyrightText: 2015 Sebastian Rettenberger <rettenbs@in.tum.de>
 */

#ifndef TRANSFER_NUMACACHE_H
#define TRANSFER_NUMACACHE_H

#include <cstring>

#include "utils/logger.h"

#include "cache/cachemanager.h"

namespace transfer {

/**
 * Copies blocks between NUMA caches.
 */
class NumaCache {
  private:
  /** NUMA domain ID of this instance */
  unsigned int m_domainId;

  /** Total number of domains */
  unsigned int m_totalDomains;

  /** Block size (in bytes) */
  unsigned long m_blockSize;

  /** List of all cache managers */
  cache::CacheManager** m_cacheManager;

  public:
  NumaCache() : m_domainId(0), m_totalDomains(0), m_blockSize(0), m_cacheManager(0L) {}

  virtual ~NumaCache() {
    if (m_domainId == 0)
      delete[] m_cacheManager;
  }

  /**
   * Initialize the transfer class
   */
  asagi::Grid::Error init(unsigned long blockSize,
                          const types::Type& type,
                          numa::NumaComm& numaComm,
                          cache::CacheManager& cacheManager) {
    m_domainId = numaComm.domainId();
    m_totalDomains = numaComm.totalDomains();
    m_blockSize = blockSize * type.size();

    // Create shared object and broadcast pointer
    if (m_domainId == 0) {
      if (numaComm.totalDomains() > 8)
        logWarning() << "More than 8 NUMA domains where detected."
                     << "Searching all NUMA caches might be slow with the current implementation.";

      m_cacheManager = new cache::CacheManager*[numaComm.totalDomains()];
    }
    asagi::Grid::Error err = numaComm.broadcast(m_cacheManager);
    if (err != asagi::Grid::SUCCESS)
      return err;

    // Set the cache manager
    m_cacheManager[m_domainId] = &cacheManager;

    return asagi::Grid::SUCCESS;
  }

  /**
   * Gets a block from the static storage of another NUMA domain
   */
  bool transfer(unsigned long blockId, unsigned char* cache) {
    for (unsigned int i = 0; i < m_totalDomains; i++) {
      if (i == m_domainId)
        continue;

      unsigned long cacheId;
      const unsigned char* remoteCache;
      if (m_cacheManager[i]->tryGet(blockId, cacheId, remoteCache)) {
        memcpy(cache, remoteCache, m_blockSize);
        m_cacheManager[i]->unlock(cacheId);
        return true;
      }
    }

    return false;
  }
};

} // namespace transfer

#endif // TRANSFER_NUMACACHE_H
