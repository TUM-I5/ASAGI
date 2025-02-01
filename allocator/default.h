/**
 * SPDX-License-Identifier: LGPLv3
 *
 * SPDX-FileCopyrightText: 2013-2015 Sebastian Rettenberger <rettenbs@in.tum.de>
 */

#ifndef ALLOCATOR_DEFAULT_H
#define ALLOCATOR_DEFAULT_H

#include "asagi.h"

namespace allocator {

/**
 * This allocator uses default C++ new/delete mechanism
 */
class Default {
  public:
  /**
   * Allocates sizeof(T)*size bytes and saves the pointer in ptr.
   *
   * @return asagi::Grid::SUCCESS if the memory was allocated
   */
  template <typename T>
  static asagi::Grid::Error allocate(size_t size, T*& ptr) {
    ptr = new T[size];
    return asagi::Grid::SUCCESS;
  }

  /**
   * Frees the memory allocated with allocate()
   */
  template <typename T>
  static void free(T* ptr) {
    delete[] ptr;
  }
};

} // namespace allocator

#endif /* ALLOCATOR_DEFAULT_H */
