/**
 * SPDX-License-Identifier: LGPLv3
 *
 * SPDX-FileCopyrightText: 2015 Sebastian Rettenberger <rettenbs@in.tum.de>
 */

#ifndef THREADS_NOOPMUTEX_H
#define THREADS_NOOPMUTEX_H

namespace threads {

/**
 * A mutex that does not lock anything.
 * This is only used if thread-safety is disabled.
 */
class NoopMutex {
  public:
  /**
   * Lock nothing
   */
  void lock() {}

  /**
   * Try lock nothing
   */
  bool try_lock() { return false; }

  /**
   * Unlock nothing
   */
  void unlock() {}
};

} // namespace threads

#endif // THREADS_NOOPMUTEX_H
