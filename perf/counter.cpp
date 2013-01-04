/**
 * @file
 *  This file is part of ASAGI
 *
 *  ASAGI is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  ASAGI is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with ASAGI.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Diese Datei ist Teil von ASAGI.
 *
 *  ASAGI ist Freie Software: Sie koennen es unter den Bedingungen
 *  der GNU General Public License, wie von der Free Software Foundation,
 *  Version 3 der Lizenz oder (nach Ihrer Option) jeder spaeteren
 *  veroeffentlichten Version, weiterverbreiten und/oder modifizieren.
 *
 *  ASAGI wird in der Hoffnung, dass es nuetzlich sein wird, aber
 *  OHNE JEDE GEWAEHELEISTUNG, bereitgestellt; sogar ohne die implizite
 *  Gewaehrleistung der MARKTFAEHIGKEIT oder EIGNUNG FUER EINEN BESTIMMTEN
 *  ZWECK. Siehe die GNU General Public License fuer weitere Details.
 *
 *  Sie sollten eine Kopie der GNU General Public License zusammen mit diesem
 *  Programm erhalten haben. Wenn nicht, siehe <http://www.gnu.org/licenses/>.
 * 
 * @copyright 2013 Sebastian Rettenberger <rettenbs@in.tum.de>
 */

#include "counter.h"

#include <cstring>

/**
 * Initializes all counters
 */
perf::Counter::Counter()
{
	memset(m_counter, 0, sizeof(m_counter));
}

/**
 * Get the current value of a counter
 *
 * @param name The name of the counter (can be a non-native type)
 * @return The value of the counter or 0 if the name is unknown
 *
 * @see NAME_TO_COUNTER
 */
unsigned long perf::Counter::get(const char* name)
{
	std::unordered_map<std::string, CounterType>::const_iterator type
		= NAME_TO_COUNTER.find(name);

	if (type == NAME_TO_COUNTER.end())
		// name not found
		return 0;

	switch (type->second) {
	case HIT:
		return m_counter[ACCESS] - m_counter[MPI] -  m_counter[FILE];
		break;
	case MISS:
		return m_counter[MPI] + m_counter[FILE];
		break;
	default:
		assert(type->second < NATIVE_COUNTER_SIZE);
		// native counters handle after switch statement
		break;
	}

	return m_counter[type->second];
}

/**
 * Maps form counter names to CounterType
 */
const std::unordered_map<std::string, perf::Counter::CounterType>
	perf::Counter::NAME_TO_COUNTER({
		{"accesses", ACCESS},
		{"mpi_transfers", MPI},
		{"file_loads", FILE},
		{"local_hits", HIT},
		{"local_misses", MISS}
	});
