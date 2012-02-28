#include "gridcontainer.h"

#include "simplegrid.h"

#include "types/basictype.h"

#include <cassert>
#include <cstring>

GridContainer::GridContainer(Type type, unsigned int hint,
	unsigned int levels)
{
	assert(levels > 0); // 0 levels don't make sense
	
	// Prepare for fortran <-> c translation
	m_id = m_pointers.add(this);
	
	m_levels = levels;
	
	m_communicator = MPI_COMM_NULL;
	
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
	
	m_valuePos = CELL_CENTERED;
	
	m_grids = new ::Grid*[levels];
	for (unsigned int i = 0; i < levels; i++)
		m_grids[i] = new SimpleGrid(*this);
}


GridContainer::~GridContainer()
{
	for (unsigned int i = 0; i < m_levels; i++)
		delete m_grids[i];
	delete [] m_grids;
	delete m_type;
	
	MPI_Comm_free(&m_communicator);
	
	// Remove from fortran <-> c translation
	m_pointers.remove(m_id);
}

asagi::Grid::Error GridContainer::init(MPI_Comm comm)
{
	if (MPI_Comm_dup(comm, &m_communicator) != MPI_SUCCESS)
		return MPI_ERROR;
	
	MPI_Comm_rank(m_communicator, &m_mpiRank);
	MPI_Comm_size(m_communicator, &m_mpiSize);
	
	return SUCCESS;
}

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
	
	assert(level < m_levels);
	// TODO let grid get parameter
	return UNKNOWN_PARAM;
}

asagi::Grid::Error GridContainer::open(const char* filename, unsigned int level)
{
	assert(level < m_levels);
	
	return m_grids[level]->open(filename);
}

double GridContainer::getXMin()
{
	// TODO check that all grids have the same range
	return m_grids[0]->getXMin();
}

double GridContainer::getYMin()
{
	// TODO check that all grids have the same range
	return m_grids[0]->getYMin();
}

double GridContainer::getZMin()
{
	// TODO check that all grids have the same range
	return m_grids[0]->getZMin();
}

double GridContainer::getXMax()
{
	// TODO check that all grids have the same range
	return m_grids[0]->getXMax();
}

double GridContainer::getYMax()
{
	// TODO check that all grids have the same range
	return m_grids[0]->getYMax();
}

double GridContainer::getZMax()
{
	// TODO check that all grids have the same range
	return m_grids[0]->getZMax();
}

char GridContainer::getByte3D(double x, double y, double z, unsigned int level)
{
	assert(level < m_levels);
	
	return m_grids[level]->getByte(x, y, z);
}

int GridContainer::getInt3D(double x, double y, double z, unsigned int level)
{
	assert(level < m_levels);
	
	return m_grids[level]->getInt(x, y, z);
}

long GridContainer::getLong3D(double x, double y, double z,
	unsigned int level)
{
	assert(level < m_levels);
	
	return m_grids[level]->getLong(x, y, z);
}

float GridContainer::getFloat3D(double x, double y, double z,
	unsigned int level)
{
	assert(level < m_levels);
	
	return m_grids[level]->getFloat(x, y, z);
}

double GridContainer::getDouble3D(double x, double y, double z,
	unsigned int level)
{
	assert(level < m_levels);
	
	return m_grids[level]->getDouble(x, y, z);
}

void GridContainer::getBuf3D(void* buf, double x, double y, double z,
	unsigned int level)
{
	assert(level < m_levels);
	
	m_grids[level]->getBuf(buf, x, y, z);
}

bool GridContainer::exportPng(const char* filename, unsigned int level)
{
	assert(level < m_levels);
	
	return m_grids[level]->exportPng(filename);
}

// Fortran <-> c translation array
fortran::PointerArray<GridContainer> GridContainer::m_pointers;
