/**
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 * SPDX-FileCopyrightText: 2015-2017 Sebastian Rettenberger <rettenbs@in.tum.de>
 */

#ifndef NUMA_NUMA_H
#define NUMA_NUMA_H

#ifndef USE_PTHREAD
#include "nonuma.h"
#else // USE_PTHREAD
#include "asagi.h"

#include <cstdint>
#include <map>
#include <pthread.h>
#include <vector>

#include "threads/sync.h"
#endif // USE_PTHREAD

namespace numa {

#ifndef USE_PTHREAD
/** NUMA dummy implementation if NUMA is disabled */
typedef NoNuma Numa;
#else // USE_PTHREAD

/**
 * Defines the size of thread and NUMA id so we can store
 * 2 IDs in one pointer
 */
#ifdef _LP64
typedef uint32_t id_t;
#else  // _LP64
typedef uint16_t id_t;
#endif // _LP64

class NumaComm;

/**
 * Detects and handles NUMA domains
 */
class Numa {
  private:
  /** Total number of threads */
  unsigned int m_totalThreads;

  /** Maps from the NUMA domain to the domain identifier */
  std::map<int, unsigned int> m_domains;

  /** Temporary array to identify if the master thread already found for a domain */
  std::vector<bool> m_masterThreads;

  /** The pthread key we use to store the domain identifier for each thread */
  pthread_key_t m_ptkey;

  /** True if the threads/domains are already initialized */
  bool m_initialized;

  /** True, if the key could not be created */
  bool m_keyError;

  /** Synchronization mechanism for the threads */
  mutable threads::Sync m_syncThreads;

  /** Synchronization mechanism for the domain */
  mutable threads::Sync m_syncDomains;

  public:
  Numa();

  virtual ~Numa();

  /**
   * Set the number of threads
   */
  asagi::Grid::Error setThreads(unsigned int threads) {
    m_totalThreads = threads;
    return asagi::Grid::SUCCESS;
  }

  asagi::Grid::Error registerThread(bool& masterThread, bool detectNumaDomains = true);

  /**
   * @return The total number of threads
   */
  unsigned int totalThreads() const { return m_totalThreads; }

  /**
   * @return The total number of NUMA domains
   */
  unsigned int totalDomains() const { return m_domains.size(); }

  /**
   * @return The id of this thread
   */
  unsigned int threadId() const {
    uintptr_t ids = reinterpret_cast<std::uintptr_t>(pthread_getspecific(m_ptkey));
    return ids >> sizeof(id_t) * 8;
  }

  /**
   * @return The id of the NUMA domain for this thread
   */
  unsigned int domainId() const {
    uintptr_t ids = reinterpret_cast<std::uintptr_t>(pthread_getspecific(m_ptkey));
    return ids & ((static_cast<uintptr_t>(1) << sizeof(id_t) * 8) - 1);
  }

  /**
   * Synchronizes all threads
   */
  asagi::Grid::Error barrier() const {
    if (!m_syncThreads.barrier(m_totalThreads))
      return asagi::Grid::THREAD_ERROR;
    return asagi::Grid::SUCCESS;
  }

  /**
   * Broadcast between all threads
   */
  template <typename T>
  asagi::Grid::Error broadcast(T& data, unsigned int root = 0) const {
    if (!m_syncThreads.broadcast(data, m_totalThreads, threadId(), root))
      return asagi::Grid::THREAD_ERROR;
    return asagi::Grid::SUCCESS;
  }

  NumaComm* createComm() const;
};

#endif // USE_PHTREAD

} // namespace numa

#endif // NUMA_NUMA_H
