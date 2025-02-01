/**
 * SPDX-License-Identifier: LGPLv3
 *
 * SPDX-FileCopyrightText: 2012-2015 Sebastian Rettenberger <rettenbs@in.tum.de>
 */

#ifndef CACHE_LRU_H
#define CACHE_LRU_H

namespace cache {

/**
 * Implements the clock page replacement algorithm with 2 hands.
 * This is a simplification of least-recently-used.
 */
class LRU {
  private:
  /** Number of "pages" that are available */
  unsigned long m_size;

  /** Next page we try */
  unsigned long m_nextPage;

  /** Next page we clear the r bit */
  unsigned long m_nextClear;

  /** Flag list for second chance */
  bool* m_referenced;

  public:
  LRU() : m_size(0), m_nextPage(0L), m_nextClear(0), m_referenced(0L) {}

  virtual ~LRU() { delete[] m_referenced; }

  /**
   * Initialize the LRU algorithm
   */
  void init(unsigned long size, long handDiff = -1) {
    m_size = size;
    m_nextPage = size - 1; // Some magic so getFree() works
    if (handDiff < 0)
      // Some default value
      m_nextClear = size / 2;
    else
      m_nextClear = handDiff;

    m_referenced = new bool[size];
    // Rest of the array will bet set to "false" by getFree()
    for (unsigned long i = 0; i < m_nextClear; i++)
      m_referenced[i] = false;
  }

  /**
   * An element is accessed, the second chance bit is set
   */
  void access(unsigned long index) { m_referenced[index] = true; }

  /**
   * Get the index of a free "page"
   */
  unsigned long getFree() {
    do { // Last page was accessed -> execute this loop at least once
      // Clear r bit
      m_referenced[m_nextClear] = false;

      // Increment both pointer
      m_nextPage = (m_nextPage + 1) % m_size;
      m_nextClear = (m_nextClear + 1) % m_size;
    } while (m_referenced[m_nextPage]);

    // We also access this page
    m_referenced[m_nextPage] = true;

    return m_nextPage;
  }
};

} // namespace cache

#endif // CACHE_LRU_H
