#include "simplegridcontainer.h"

#include "nompigrid.h"
#ifndef ASAGI_NOMPI
#include "simplegrid.h"
#include "largegrid.h"
#endif // ASAGI_NOMPI

#include <algorithm>

SimpleGridContainer::SimpleGridContainer(asagi::Grid::Type type, bool isArray,
	unsigned int hint, unsigned int levels)
	: GridContainer(type, isArray, hint, levels)
{
	m_grids = new ::Grid*[m_levels];
#ifdef ASAGI_NOMPI
	for (unsigned int i = 0; i < levels; i++)
		m_grids[i] = new NoMPIGrid(*this, hint);
#else // ASAGI_NOMPI
	if (hint & asagi::LARGE_GRID) {
		for (unsigned int i = 0; i < levels; i++)
			m_grids[i] = new LargeGrid(*this, hint, i);
	} else {
		for (unsigned int i = 0; i < levels; i++)
			m_grids[i] = new SimpleGrid(*this, hint);
	}
#endif // ASAGI_NOMPI
}

SimpleGridContainer::~SimpleGridContainer()
{
	for (unsigned int i = 0; i < m_levels; i++)
		delete m_grids[i];
	delete [] m_grids;
}

asagi::Grid::Error SimpleGridContainer::setParam(const char* name,
	const char* value, unsigned int level)
{
	Error result;
	
	result = GridContainer::setParam( name, value, level);
	if (result != UNKNOWN_PARAM)
		return result;
	
	assert(level < m_levels);
	return m_grids[level]->setParam(name, value);
}

asagi::Grid::Error SimpleGridContainer::open(const char* filename,
	unsigned int level)
{
	Error result;
	
	result = GridContainer::open(filename, level);
	if (result != SUCCESS)
		return result;
	
	result = m_grids[level]->open(filename);
	if (result != SUCCESS)
		return result;
	
	m_minX = std::max(m_minX, m_grids[level]->getXMin());
	m_minY = std::max(m_minY, m_grids[level]->getYMin());
	m_minZ = std::max(m_minZ, m_grids[level]->getZMin());
	
	m_maxX = std::min(m_maxX, m_grids[level]->getXMax());
	m_maxY = std::min(m_maxY, m_grids[level]->getYMax());
	m_maxZ = std::min(m_maxZ, m_grids[level]->getZMax());
	
	return result;
}

char SimpleGridContainer::getByte3D(double x, double y, double z, unsigned int level)
{
	assert(level < m_levels);
	
	return m_grids[level]->getByte(x, y, z);
}

int SimpleGridContainer::getInt3D(double x, double y, double z, unsigned int level)
{
	assert(level < m_levels);
	
	return m_grids[level]->getInt(x, y, z);
}

long SimpleGridContainer::getLong3D(double x, double y, double z,
	unsigned int level)
{
	assert(level < m_levels);
	
	return m_grids[level]->getLong(x, y, z);
}

float SimpleGridContainer::getFloat3D(double x, double y, double z,
	unsigned int level)
{
	assert(level < m_levels);
	
	return m_grids[level]->getFloat(x, y, z);
}

double SimpleGridContainer::getDouble3D(double x, double y, double z,
	unsigned int level)
{
	assert(level < m_levels);
	
	return m_grids[level]->getDouble(x, y, z);
}

void SimpleGridContainer::getBuf3D(void* buf, double x, double y, double z,
	unsigned int level)
{
	assert(level < m_levels);
	
	m_grids[level]->getBuf(buf, x, y, z);
}

bool SimpleGridContainer::exportPng(const char* filename, unsigned int level)
{
	assert(level < m_levels);
	
	return m_grids[level]->exportPng(filename);
}

