/**
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 * SPDX-FileCopyrightText: 2013-2015 Sebastian Rettenberger <rettenbs@in.tum.de>
 */

#include "counter.h"

#include <cstring>

/**
 * Initializes all counters
 */
perf::Counter::Counter() { memset(m_counter, 0, sizeof(m_counter)); }

/**
 * Get the current value of a counter
 *
 * @param type The counter type (can be a non-native type)
 * @return The value of the counter or 0 if the name is unknown
 *
 * @see name2type
 */
unsigned long perf::Counter::get(CounterType type) const {
  switch (type) {
  case HIT:
    return m_counter[ACCESS] - m_counter[NUMA] - m_counter[MPI] - m_counter[FILE];
  case NODE_HIT:
    return m_counter[ACCESS] - m_counter[MPI] - m_counter[FILE];
  case MISS:
    return m_counter[NUMA] + m_counter[MPI] + m_counter[FILE];
  case INVALID:
    return 0;
  default:
    assert(type < NATIVE_COUNTER_SIZE);
    // native counters handle after switch statement
  }

  return m_counter[type];
}

/**
 * Converts a string representation of the counter to the counter type
 *
 * @param name The name of the type
 * @return The corresponding type
 */
perf::Counter::CounterType perf::Counter::name2type(const char* name) {
  std::unordered_map<std::string, CounterType>::const_iterator type = NAME_TO_COUNTER.find(name);

  if (type == NAME_TO_COUNTER.end())
    // name not found
    return INVALID;

  return type->second;
}

/**
 * Maps form counter names to CounterType
 */
const perf::Counter::NameToCounterMap perf::Counter::NAME_TO_COUNTER;
// Not yet supported by icc
// const std::unordered_map<std::string, perf::Counter::CounterType>
//	perf::Counter::NAME_TO_COUNTER({
//		{"accesses", ACCESS},
//		{"mpi_transfers", MPI},
//		{"file_loads", FILE},
//		{"local_hits", HIT},
//		{"local_misses", MISS}
//	});
