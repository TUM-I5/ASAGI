/**
 * @file
 *  This file is part of ASAGI.
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
 * @copyright 2012 Sebastian Rettenberger <rettenbs@in.tum.de>
 */

#include "gridcontainer.h"

#include "grid/localcachegrid.h"
#include "grid/staticgrid.h"
#ifndef ASAGI_NOMPI
#include "grid/mpicachegrid.h"
#include "grid/largegrid.h"
#endif // ASAGI_NOMPI

#include "types/arraytype.h"
#include "types/basictype.h"
#include "types/structtype.h"

#include <cassert>
#include <cstring>
#include <limits>

/**
 * @param type The basic type of the values
 * @param isArray True if the type is an array, false if it is a basic
 *  type
 * @param hint Any performance hints
 * @param levels The number of levels
 */
grid::GridContainer::GridContainer(Type type, bool isArray, unsigned int hint,
	unsigned int levels)
	: m_levels(levels)
{
	assert(levels > 0); // 0 levels don't make sense
	
	// Prepare for fortran <-> c translation
	m_id = m_pointers.add(this);
	
#ifdef ASAGI_NOMPI
	m_noMPI = true;
#else // ASAGI_NOMPI
	m_noMPI = hint & NOMPI;
	m_communicator = MPI_COMM_NULL;
#endif // ASAGI_NOMPI
	
	if (isArray) {
		switch (type) {
		case BYTE:
			m_type = new types::ArrayType<char>();
			break;
		case INT:
			m_type = new types::ArrayType<int>();
			break;
		case LONG:
			m_type = new types::ArrayType<long>();
			break;
		case FLOAT:
			m_type = new types::ArrayType<float>();
			break;
		case DOUBLE:
			m_type = new types::ArrayType<double>();
			break;
		default:
			m_type = 0L;
			assert(false);
			break;
		}
	} else {
		switch (type) {
		case BYTE:
			m_type = new types::BasicType<char>();
			break;
		case INT:
			m_type = new types::BasicType<int>();
			break;
		case LONG:
			m_type = new types::BasicType<long>();
			break;
		case FLOAT:
			m_type = new types::BasicType<float>();
			break;
		case DOUBLE:
			m_type = new types::BasicType<double>();
			break;
		default:
			m_type = 0L;
			assert(false);
			break;
		}
	}
	
	m_minX = m_minY = m_minZ = -std::numeric_limits<double>::infinity();
	m_maxX = m_maxY = m_maxZ = std::numeric_limits<double>::infinity();
	
	m_valuePos = CELL_CENTERED;
	
	// Default values (probably only useful when compiled without MPI support)
	m_mpiRank = 0;
	m_mpiSize = 1;
}

grid::GridContainer::GridContainer(unsigned int count,
		unsigned int blockLength[],
		unsigned long displacements[],
		asagi::Grid::Type types[],
		unsigned int hint, unsigned int levels)
	: m_levels(levels)
{
	assert(levels > 0); // 0 levels don't make sense
	
	// Prepare for fortran <-> c translation
	m_id = m_pointers.add(this);
	
#ifdef ASAGI_NOMPI
	m_noMPI = true;
#else // ASAGI_NOMPI
	m_noMPI = hint & NOMPI;
	m_communicator = MPI_COMM_NULL;
#endif // ASAGI_NOMPI
	
	m_type = types::createStruct(count, blockLength,
		displacements, types);
	
	m_minX = m_minY = m_minZ = -std::numeric_limits<double>::infinity();
	m_maxX = m_maxY = m_maxZ = std::numeric_limits<double>::infinity();
	
	m_valuePos = CELL_CENTERED;
	
	// Default values (probably only useful when compiled without MPI support)
	m_mpiRank = 0;
	m_mpiSize = 1;
}


grid::GridContainer::~GridContainer()
{
	delete m_type;
	
#ifndef ASAGI_NOMPI
	if (m_communicator != MPI_COMM_NULL)
		MPI_Comm_free(&m_communicator);
#endif // ASAGI_NOMPI
	
	// Remove from fortran <-> c translation
	m_pointers.remove(m_id);
}

#ifndef ASAGI_NOMPI
asagi::Grid::Error grid::GridContainer::setComm(MPI_Comm comm)
{
	if (m_communicator != MPI_COMM_NULL)
		// set communicator only once
		return SUCCESS;

	if (MPI_Comm_dup(comm, &m_communicator) != MPI_SUCCESS)
		return MPI_ERROR;

	MPI_Comm_rank(m_communicator, &m_mpiRank);
	MPI_Comm_size(m_communicator, &m_mpiSize);

	return SUCCESS;
}
#endif // ASAGI_NOMPI

asagi::Grid::Error grid::GridContainer::setParam(const char* name,
	const char* value,
	unsigned int level)
{
	if (strcmp(name, "value-position") == 0) {
		if (strcmp(value, "cell-centered") == 0) {
			m_valuePos = CELL_CENTERED;
			return SUCCESS;
		}
		
		if (strcmp(value, "vertex-centered") == 0) {
			m_valuePos = VERTEX_CENTERED;
			return SUCCESS;
		}
		
		return INVALID_VALUE;
	}
	
	return UNKNOWN_PARAM;
}

asagi::Grid::Error grid::GridContainer::open(const char* filename,
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
grid::Grid* grid::GridContainer::createGrid(unsigned int hint,
	unsigned int id) const
{
#ifndef ASAGI_NOMPI
	if (hint & NOMPI) {
#endif // ASAGI_NOMPI
		if (hint & SMALL_CACHE)
			return new LocalCacheGrid(*this, hint);

		return new StaticGrid(*this, hint);
#ifndef ASAGI_NOMPI
	}
#endif // ASAGI_NOMPI

#ifndef ASAGI_NOMPI
	if (hint & LARGE_GRID)
		return new LargeGrid(*this, hint, id);

	return new MPICacheGrid(*this, hint);
#endif // ASAGI_NOMPI
}

// Fortran <-> c translation array
fortran::PointerArray<grid::GridContainer>
	grid::GridContainer::m_pointers;
