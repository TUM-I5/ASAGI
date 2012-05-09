#include "adaptivegridcontainer.h"

#include "nompigrid.h"
#ifndef ASAGI_NOMPI
#include "simplegrid.h"
#include "largegrid.h"
#endif // ASAGI_NOMPI

AdaptiveGridContainer::AdaptiveGridContainer(asagi::Grid::Type type,
	bool isArray, unsigned int hint, unsigned int levels)
	: GridContainer(type, isArray, hint, levels),
	m_hint(hint)
{
	m_grids = new std::vector< ::Grid*>[m_levels];
	
	m_ids = 0;
}


AdaptiveGridContainer::~AdaptiveGridContainer()
{
	std::vector< ::Grid*>::const_iterator grid;
	
	for (unsigned int i = 0; i < m_levels; i++) {
		for (grid = m_grids[i].begin(); grid < m_grids[i].end(); grid++)
			delete *grid;
	}
	delete [] m_grids;
}

asagi::Grid::Error AdaptiveGridContainer::open(const char* filename,
	unsigned int level)
{
	Error result;
	::Grid* grid;
	
	result = GridContainer::open(filename, level);
	if (result != SUCCESS)
		return result;
	
#ifdef THREADSAFETY
	std::lock_guard<std::mutex> lock(m_mutex);
#endif // THREADSAFETY
	
	if (level == 0 && !m_grids[0].empty())
		// There must not be more than one level 0 grid
		return MULTIPLE_TOPGRIDS;
	
#ifdef ASAGI_NOMPI
	grid = new NoMPIGrid(*this, m_hint);
#else // ASAGI_NOMPI
	if (m_hint & asagi::LARGE_GRID) {
		grid = new LargeGrid(*this, m_hint, m_ids++);
	} else {
		grid = new SimpleGrid(*this, m_hint);
	}
#endif // ASAGI_NOMPI

	result = grid->open(filename);
	if (result != SUCCESS)
		return result;

	m_grids[level].push_back(grid);
	
	if (level == 0) {
		// Set min/max from level 0 grid
		m_minX = grid->getXMin();
		m_minY = grid->getYMin();
		m_minZ = grid->getZMin();
		
		m_maxX = grid->getXMax();
		m_maxY = grid->getYMax();
		m_maxZ = grid->getZMax();
	}
	
	return SUCCESS;
}

char AdaptiveGridContainer::getByte3D(double x, double y, double z, unsigned int level)
{
	return getGrid(x, y, z, level)->getByte(x, y, z);
}

int AdaptiveGridContainer::getInt3D(double x, double y, double z, unsigned int level)
{
	return getGrid(x, y, z, level)->getInt(x, y, z);
}

long AdaptiveGridContainer::getLong3D(double x, double y, double z,
	unsigned int level)
{
	return getGrid(x, y, z, level)->getLong(x, y, z);
}

float AdaptiveGridContainer::getFloat3D(double x, double y, double z,
	unsigned int level)
{
	return getGrid(x, y, z, level)->getFloat(x, y, z);
}

double AdaptiveGridContainer::getDouble3D(double x, double y, double z,
	unsigned int level)
{
	return getGrid(x, y, z, level)->getDouble(x, y, z);
}

void AdaptiveGridContainer::getBuf3D(void* buf, double x, double y, double z,
	unsigned int level)
{
	getGrid(x, y, z, level)->getBuf(buf, x, y, z);
}

bool AdaptiveGridContainer::exportPng(const char* filename, unsigned int level)
{
	assert(level < m_levels);
	
	if (level != 0)
		// Sry, but we can not export subgrids ...
		return false;
	
	return m_grids[0][0]->exportPng(filename);
}

/**
 *
 */
::Grid* AdaptiveGridContainer::getGrid(double x, double y, double z,
	unsigned int level)
{
	std::vector< ::Grid*>::const_iterator grid;
	
	assert(level < m_levels);
	assert(m_minX <= x && m_maxX >= x && m_minY <= y && m_maxY >= y
		&& m_minZ <= z && m_maxZ >= z);
	
	for (size_t i = level; i > 0; i--) {
		for (grid = m_grids[i].begin(); grid < m_grids[i].end(); grid++) {
			if ((*grid)->getXMin() <= x && (*grid)->getXMax() >= x
				&& (*grid)->getYMin() <= x && (*grid)->getYMax() >= y
				&& (*grid)->getXMin() <= x && (*grid)->getZMax() >= z)
				return *grid;
		}
	}
	
	return m_grids[0][0];
}
