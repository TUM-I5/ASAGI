/**
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 * SPDX-FileCopyrightText: 2012-2015 Sebastian Rettenberger <rettenbs@in.tum.de>
 */

#ifndef FORTRAN_POINTERARRAY_H
#define FORTRAN_POINTERARRAY_H

#include <cassert>
#include <mutex>
#include <vector>

#include "threads/mutex.h"

/**
 * @brief C++ code required to support the Fortran API
 */
namespace fortran {

/**
 * Maps indices to pointers. The reverse mapping (from pointers to indices) is
 * done by the class itself.
 */
template <class T>
class PointerArray {
  private:
  /** Array that maps to the correct pointer */
  std::vector<T*> m_pointers;

  /**
   * Lock, we use to make sure only one thread at a time adds a pointer
   */
  threads::Mutex m_lock;

  public:
  /**
   * Add a new pointer to the map
   *
   * @return The index which can be used to access the pointer
   */
  int add(T* const p) {
    // Lock vector, otherwise the id (return value)
    // gets messed up
    std::lock_guard<threads::Mutex> lock(m_lock);

    m_pointers.push_back(p);
    return m_pointers.size() - 1;
  }

  /**
   * @return The pointer to the object for index <code>i</code>
   */
  T* get(int i) const {
    assert(i >= 0 && static_cast<unsigned int>(i) < m_pointers.size());

    return m_pointers[i];
  }

  /**
   * Removes the pointer with index <code>i</code> from the map
   */
  void remove(int i) {
    assert(i >= 0 && static_cast<unsigned int>(i) < m_pointers.size());

    m_pointers[i] = 0L;
  }
};

} // namespace fortran

#endif // FORTRAN_POINTERARRAY_H
