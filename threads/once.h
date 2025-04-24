/**
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 * SPDX-FileCopyrightText: 2015 Sebastian Rettenberger <rettenbs@in.tum.de>
 */

#ifndef THREADS_ONCE_H
#define THREADS_ONCE_H

#include <mutex>

#include "mutex.h"

namespace threads {

/**
 * A small class that executes a function exactly once
 */
class Once {
  private:
  /** Already executed */
  bool m_state;

  /** Mutex to lock the state */
  Mutex m_mutex;

  public:
  Once() : m_state(false) {}

  /**
   * Executes <code>func</code> of <code>obj</code>
   * exactly once. When this function returns, <code>func</code>
   * has been executed.
   */
  template <class T>
  void saveExec(T& obj, void (T::*func)()) {
    // Default case
    if (m_state)
      return;

    // Lock the state and double check
    std::lock_guard<Mutex> lock(m_mutex);
    if (m_state)
      return;

    // Save execute:
    // Execute the function before freeing the lock
    (obj.*func)();

    m_state = true;
  }
};

} // namespace threads

#endif // THREADS_ONCE_H
