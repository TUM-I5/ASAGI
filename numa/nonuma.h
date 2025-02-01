/**
 * SPDX-License-Identifier: LGPLv3
 *
 * SPDX-FileCopyrightText: 2015-2017 Sebastian Rettenberger <rettenbs@in.tum.de>
 */

#ifndef NUMA_NONUMA_H
#define NUMA_NONUMA_H

#include "asagi.h"

namespace numa {

class NumaComm;

/**
 * Dummy implementation for {@link Numa} if NUMA support is disabled
 */
class NoNuma {
  public:
  NoNuma() {}

  virtual ~NoNuma() {}

  /**
   * Set number of threads (should be 1)
   */
  asagi::Grid::Error setThreads(unsigned int threads) { return asagi::Grid::SUCCESS; }

  /**
   * @param[out] masterThread Will be set to <code>true</code> since only
   *  one thread should exist.
   */
  asagi::Grid::Error registerThread(bool& masterThread) {
    masterThread = true;
    return asagi::Grid::SUCCESS;
  }

  /**
   * @return Always 1
   */
  unsigned int totalThreads() const { return 1; }

  /**
   * @return Always 1
   */
  unsigned int totalDomains() const { return 1; }

  /**
   * @return Always 0
   */
  unsigned int threadId() const { return 0; }

  /**
   * @return Always 0
   */
  unsigned int domainId() const { return 0; }

  /**
   * Does nothing (not useful for one thread)
   */
  asagi::Grid::Error barrier() const { return asagi::Grid::SUCCESS; }

  /**
   * Does nothing (not useful for one thread)
   */
  template <typename T>
  asagi::Grid::Error broadcast(T& data, unsigned int root = 0) const {
    return asagi::Grid::SUCCESS;
  }

  NumaComm* createComm() const;
};

} // namespace numa

#endif // NUMA_NONUMA_H
