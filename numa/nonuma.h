/**
 * @file
 *  This file is part of ASAGI.
 *
 *  ASAGI is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as
 *  published by the Free Software Foundation, either version 3 of
 *  the License, or  (at your option) any later version.
 *
 *  ASAGI is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with ASAGI.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Diese Datei ist Teil von ASAGI.
 *
 *  ASAGI ist Freie Software: Sie koennen es unter den Bedingungen
 *  der GNU Lesser General Public License, wie von der Free Software
 *  Foundation, Version 3 der Lizenz oder (nach Ihrer Option) jeder
 *  spaeteren veroeffentlichten Version, weiterverbreiten und/oder
 *  modifizieren.
 *
 *  ASAGI wird in der Hoffnung, dass es nuetzlich sein wird, aber
 *  OHNE JEDE GEWAEHELEISTUNG, bereitgestellt; sogar ohne die implizite
 *  Gewaehrleistung der MARKTFAEHIGKEIT oder EIGNUNG FUER EINEN BESTIMMTEN
 *  ZWECK. Siehe die GNU Lesser General Public License fuer weitere Details.
 *
 *  Sie sollten eine Kopie der GNU Lesser General Public License zusammen
 *  mit diesem Programm erhalten haben. Wenn nicht, siehe
 *  <http://www.gnu.org/licenses/>.
 *
 * @copyright 2015 Sebastian Rettenberger <rettenbs@in.tum.de>
 */

#ifndef NUMA_NONUMA_H
#define NUMA_NONUMA_H

#include "asagi.h"

namespace numa
{

class NumaComm;

/**
 * Dummy implementation for {@link Numa} if NUMA support is disabled
 */
class NoNuma
{
public:
	NoNuma()
	{
	}

	virtual ~NoNuma()
	{
	}


	/**
	 * Set number of threads (should be 1)
	 */
	asagi::Grid::Error setThreads(unsigned int threads)
	{
		return asagi::Grid::SUCCESS;
	}

	/**
	 * @param[out] masterThread Will be set to <code>true</code> since only
	 *  one thread should exist.
	 */
	asagi::Grid::Error registerThread(bool &masterThread)
	{
		masterThread = true;
		return asagi::Grid::SUCCESS;
	}

	/**
	 * @return Always 1
	 */
	unsigned int totalThreads() const
	{
		return 1;
	}

	/**
	 * @return Always 1
	 */
	unsigned int totalDomains() const
	{
		return 1;
	}

	/**
	 * @return Always 0
	 */
	unsigned int threadId() const
	{
		return 0;
	}

	/**
	 * @return Always 0
	 */
	unsigned int domainId() const
	{
		return 0;
	}

	/**
	 * Does nothing (not useful for one thread)
	 */
	asagi::Grid::Error barrier() const
	{
		return asagi::Grid::SUCCESS;
	}

	NumaComm* createComm() const;

};

}

#endif // NUMA_NONUMA_H
