/**
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 * SPDX-FileCopyrightText: 2015 Sebastian Rettenberger <rettenbs@in.tum.de>
 */

#ifndef NUMA_NUMACOMM_H
#define NUMA_NUMACOMM_H

#include "asagi.h"

#include "numa.h"
#include "threads/sync.h"

namespace numa {

/**
 * Implements NUMA communication functions
 */
class NumaComm {
  private:
  /** NUMA detection */
  const Numa& m_numa;

  /** Synchronization mechanism */
  threads::Sync m_sync;

  public:
  /**
   * Use {@link Numa::createComm()} to create an instance of
   * this class.
   */
  NumaComm(const Numa& numa) : m_numa(numa) {}

  virtual ~NumaComm() {}

  /**
   * @copydoc Numa::totalThreads
   */
  unsigned int totalThreads() const { return m_numa.totalThreads(); }

  /**
   * @copydoc Numa::totalDomains
   */
  unsigned int totalDomains() const { return m_numa.totalDomains(); }

  /**
   * @copydoc Numa::threadId
   */
  unsigned int threadId() const { return m_numa.threadId(); }

  /**
   * @copydoc Numa::domainId
   */
  unsigned int domainId() const { return m_numa.domainId(); }

  /**
   * Broadcast a value to all domain master.
   * This is a collective operation among the NUMA masters.
   *
   * @param value The value that should be broadcasted
   * @param rootDomain The thread (domainId of the thread) that holds
   *  the value
   */
  template <typename T>
  asagi::Grid::Error broadcast(T& value, unsigned int rootDomain = 0) {
    if (!m_sync.broadcast(value, totalDomains(), domainId(), rootDomain))
      return asagi::Grid::THREAD_ERROR;

    return asagi::Grid::SUCCESS;
  }

  /**
   * Allocates memory for all NUMA domains. This is a collective
   * operation among the NUMA masters.
   *
   * @param size Number of elements allocated for each NUMA domain
   * @param[out] data Pointer to the allocated memory
   */
  template <class Allocator, typename T>
  asagi::Grid::Error allocate(unsigned long size, T*& data) {
    if (domainId() == 0) {
      // Allocate the memory with the master thread
      asagi::Grid::Error err = Allocator::allocate(size * totalDomains(), data);
      if (err != asagi::Grid::SUCCESS)
        return err;
    }

    // Broadcast the pointer to all domains
    asagi::Grid::Error err = broadcast(data);
    if (err != asagi::Grid::SUCCESS)
      return err;

    // Get the local pointer
    data = &data[size * domainId()];

    return asagi::Grid::SUCCESS;
  }

  /**
   * Frees the the memory allocated with {@link allocate()}
   *
   * @param data Pointer to the allocated memory
   *
   * @warning The caller has to make sure that is is only called from one thread.
   */
  template <class Allocator, typename T>
  void free(T* data) const {
    Allocator::free(data);
  }

  /**
   * Creates a copy of this communicator using {@link Numa::createComm}.
   */
  NumaComm* copy() const { return m_numa.createComm(); }
};

} // namespace numa

#endif // NUMA_NUMACOMM_H
