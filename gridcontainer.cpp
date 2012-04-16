#include "gridcontainer.h"

#include "types/arraytype.h"
#include "types/basictype.h"

#include <cassert>
#include <cstring>
#include <limits>

GridContainer::GridContainer(Type type, bool isArray, unsigned int hint,
	unsigned int levels)
	: m_levels(levels)
{
	assert(levels > 0); // 0 levels don't make sense
	
	// Prepare for fortran <-> c translation
	m_id = m_pointers.add(this);
	
#ifndef ASAGI_NOMPI
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
		}
	}
	
	m_minX = m_minY = m_minZ = -std::numeric_limits<double>::infinity();
	m_maxX = m_maxY = m_maxZ = std::numeric_limits<double>::infinity();
	
	m_valuePos = CELL_CENTERED;
	
	// Default values (probably only useful when compiled without MPI support)
	m_mpiRank = 0;
	m_mpiSize = 1;
}


GridContainer::~GridContainer()
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
asagi::Grid::Error GridContainer::setComm(MPI_Comm comm)
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

asagi::Grid::Error GridContainer::setParam(const char* name, const char* value,
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

asagi::Grid::Error GridContainer::open(const char* filename, unsigned int level)
{
	assert(level < m_levels);
	
#ifdef ASAGI_NOMPI
	return SUCCESS;
#else // ASAGI_NOMPI
	return setComm(); // Make sure we have our own communicator
#endif // ASAGI_NOMPI
}

// Fortran <-> c translation array
fortran::PointerArray<GridContainer> GridContainer::m_pointers;
