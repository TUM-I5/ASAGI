/**
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 * SPDX-FileCopyrightText: 2015 Sebastian Rettenberger <rettenbs@in.tum.de>
 */

#ifndef THREADS_MUTEX_H
#define THREADS_MUTEX_H

#ifdef THREADSAFE

#ifdef USE_PTHREAD
#include "pthreadmutex.h"

namespace threads {
/** Mutex type depending on the compilation flags */
typedef PthreadMutex Mutex;
} // namespace threads

#else // USE_PTHREAD
#include "cxxmutex.h"

namespace threads {
typedef CxxMutex Mutex;
}

#endif // USE_PTHREAD

#else // THREADSAFE
#include "noopmutex.h"

namespace threads {
typedef NoopMutex Mutex;
}

#endif // THREADSAFE

#endif // THREADS_MUTEX_H
