/**
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 * SPDX-FileCopyrightText: 2012-2015 Sebastian Rettenberger <rettenbs@in.tum.de>
 */

#ifndef CACHE_CACHELIST_H
#define CACHE_CACHELIST_H

#include <unordered_map>

#include "lru.h"

/**
 * @brief Algorithms to handle the local block cache
 */
namespace cache {

/**
 * @brief Controls a list of blocks
 *
 * It does not store the blocks itself, it only handles the free spaces in the
 * list and maps between block index and list position.
 */
class CacheList {
  private:
  /** Algorithm we use to delete old blocks */
  LRU m_lru;

  /**
   * Maps from the local index of the block to the real block id.
   * A block id < 0 means that this index is empty
   */
  long* m_indexToBlock;

  /** Maps from the block id to the index where it is stored */
  std::unordered_map<unsigned long, unsigned long> m_blockToIndex;

  public:
  CacheList() : m_indexToBlock(0L) {}

  virtual ~CacheList() { delete[] m_indexToBlock; }

  /**
   * @param size The number of elements in the cache
   * @param handDiff Difference between the two hands in the
   *  2-handed clock algorithm
   */
  void init(unsigned long size, long handDiff = -1) {
    m_indexToBlock = new long[size];
    for (unsigned long i = 0; i < size; i++)
      m_indexToBlock[i] = -1;

    m_lru.init(size, handDiff);
  }

  /**
   * @param block The global block id
   * @param[out] index If true, the local index of this block,
   *  otherwise the value is undefined
   * @param access True if the blocked should be marked as accessed
   * @return True if this block is stored, false otherwise
   */
  bool getIndex(unsigned long block, unsigned long& index, bool access = true) {
    std::unordered_map<unsigned long, unsigned long>::const_iterator value =
        m_blockToIndex.find(block);

    if (value == m_blockToIndex.end())
      return false;

    index = (*value).second;
    if (access)
      m_lru.access(index);

    return true;
  }

  /**
   * @param block The id of the new block
   * @param index The index, where the block should be saved
   * @return The id of the block that was deleted
   */
  long getFreeIndex(unsigned long block, unsigned long& index) {
    index = m_lru.getFree();
    long oldBlock = m_indexToBlock[index];

    if (oldBlock >= 0) {
      // This block is not empty
      // -> delete the old block

      m_blockToIndex.erase(oldBlock);
    }

    m_indexToBlock[index] = block;
    m_blockToIndex[block] = index;

    return oldBlock;
  }
};

} // namespace cache

#endif // CACHE_CACHELIST_H
