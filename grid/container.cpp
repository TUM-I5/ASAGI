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
 * @copyright 2012-2015 Sebastian Rettenberger <rettenbs@in.tum.de>
 */

#include "container.h"

#if 0
#include "grid/localcachegrid.h"
#include "grid/localstaticgrid.h"
#include "grid/passthroughgrid.h"

#ifndef ASAGI_NOMPI
#include "grid/diststaticgrid.h"
#include "grid/distcachegrid.h"
#endif // ASAGI_NOMPI

#include "types/arraytype.h"
#include "types/basictype.h"
#include "types/structtype.h"

#include <cassert>
#include <cstring>
#include <limits>
#endif

grid::Container::Container(const mpi::MPIComm &comm,
		const numa::Numa &numa,
		int timeDimension,
		ValuePosition valuePos)
	: m_comm(comm), m_numa(numa),
	  m_timeDimension(timeDimension), m_valuePos(valuePos)
{
}

grid::Container::~Container()
{
}

#if 0
asagi::Grid::Error grid::Container::open(const char* filename,
	unsigned int level)
{
	assert(level < m_levels);
	
#ifdef ASAGI_NOMPI
	return SUCCESS;
#else // ASAGI_NOMPI
	// Make sure we have our own communicator
	if (m_noMPI)
		return SUCCESS;
	return setComm();
#endif // ASAGI_NOMPI
}

/**
 * Creates a new grid according to the hints
 */
grid::Grid2* grid::GridContainer::createGrid(unsigned int hint,
	unsigned int id) const
{
	if (hint & PASS_THROUGH)
		return new PassThroughGrid(*this, hint);

#ifndef ASAGI_NOMPI
	if (hint & NOMPI) {
#endif // ASAGI_NOMPI
		if (hint & SMALL_CACHE)
			return new LocalCacheGrid(*this, hint);

		return new LocalStaticGrid(*this, hint);
#ifndef ASAGI_NOMPI
	}
#endif // ASAGI_NOMPI

#ifndef ASAGI_NOMPI
	if (hint & LARGE_GRID)
		return new DistCacheGrid(*this, hint, id);

	return new DistStaticGrid(*this, hint);
#endif // ASAGI_NOMPI
}
#endif
