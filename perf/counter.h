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

#ifndef PERF_COUNTER_H
#define PERF_COUNTER_H

#include <cassert>
#include <string>
#include <unordered_map>

/**
 * Performance messurement tools
 */
namespace perf
{

/**
 * Stores the performance counters for one grid level
 */
class Counter
{
private:
	/* Number of available native counters */
	static const unsigned int NATIVE_COUNTER_SIZE = 3;

public:
	/** Available counters */
	enum CounterType {
		/** Total number of accesses (native) */
		ACCESS,
		/** Local cache misses but remote hits (native) */
		MPI,
		/** Cache misses; fallback to file (native) */
		FILE,
		/** Total number of local hits (ACCESS - MPI - FILE) */
		HIT,
		/** Total number of local misses (MPI + FILE) */
		MISS,
		/** Unknown counter */
		INVALID
	};

private:
	/** Stores native counters */
	unsigned long m_counter[NATIVE_COUNTER_SIZE];

public:
	Counter();

	/**
	 * Increment a counter
	 *
	 * @param type A native counter that should be incremented
	 */
	void inc(CounterType type)
	{
		// We should only increment native counters
		assert(type < NATIVE_COUNTER_SIZE);

		m_counter[type]++;
	}

	unsigned long get(const char* name);

private:
	static const std::unordered_map<std::string, CounterType> NAME_TO_COUNTER;
};

}

#endif /* PERF_COUNTER_H */
