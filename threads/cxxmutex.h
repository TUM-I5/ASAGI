/**
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 * SPDX-FileCopyrightText: 2015 Sebastian Rettenberger <rettenbs@in.tum.de>
 */

#ifndef THREADS_CXXMUTEX_H
#define THREADS_CXXMUTEX_H

#include <mutex>

namespace threads {

/** Rename of {@link std::mutex} to match the ASAGI names. */
typedef std::mutex CxxMutex;

} // namespace threads

#endif // THREADS_CXXMUTEX_H
