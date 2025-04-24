/**
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 * SPDX-FileCopyrightText: 2015 Sebastian Rettenberger <rettenbs@in.tum.de>
 */

#ifndef THREADS_PTHREADMUTEX_H
#define THREADS_PTHREADMUTEX_H

#ifndef USE_PTHREAD
#error "PThreads are not enabled."
#endif // USE_PTHREAD

#include <pthread.h>

namespace threads {

/**
 * Mutex based on pthread spin locks
 */
class PthreadMutex {
  private:
  /** The pthread spin lock */
#ifndef __APPLE__
  pthread_spinlock_t m_lock;
#else
  pthread_mutex_t m_lock;
#endif // _APPLE__

  public:
#ifndef __APPLE__
  PthreadMutex() { pthread_spin_init(&m_lock, PTHREAD_PROCESS_PRIVATE); }

  ~PthreadMutex() { pthread_spin_destroy(&m_lock); }

  /**
   * Lock the mutex
   */
  void lock() { pthread_spin_lock(&m_lock); }

  /**
   * Try to lock the mutex
   */
  bool try_lock() { return pthread_spin_trylock(&m_lock) == 0; }

  /**
   * Unlock the mutex
   */
  void unlock() { pthread_spin_unlock(&m_lock); }
#else
  PthreadMutex() {
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_PROCESS_PRIVATE);
    pthread_mutex_init(&m_lock, &attr);
  }

  ~PthreadMutex() { pthread_mutex_destroy(&m_lock); }

  /**
   * Lock the mutex
   */
  void lock() { pthread_mutex_lock(&m_lock); }

  /**
   * Try to lock the mutex
   */
  bool try_lock() { return pthread_mutex_trylock(&m_lock) == 0; }

  /**
   * Unlock the mutex
   */
  void unlock() { pthread_mutex_unlock(&m_lock); }
#endif // __APPLE__
};

} // namespace threads

#endif // THREADS_PTHREADMUTEX_H
