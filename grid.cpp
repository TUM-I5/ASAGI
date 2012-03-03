#include "grid.h"
#include "gridcontainer.h"

#include <math.h>
#include <limits>

#ifdef PNG_ENABLED
#include "io/png.h"
#endif

#include "types/basictype.h"
#include "debug/dbg.h"

// TODO
#define BLOCKS_PER_NODE 80

using namespace io;

Grid::Grid(GridContainer &container)
	: m_container(container), m_variableName("z")
{
	
	m_inputFile = 0L;
	
	// Set defaul block size
	m_blockSizeX = m_blockSizeY = m_blockSizeZ = 50;
}

Grid::~Grid()
{
	delete m_inputFile;
}

asagi::Grid::Error Grid::setParam(const char* name, const char* value)
{
	if (strcmp(name, "variable-name") == 0) {
		m_variableName = value;
		return asagi::Grid::SUCCESS;
	}
	
	return asagi::Grid::UNKNOWN_PARAM;
}

asagi::Grid::Error Grid::open(const char* filename)
{
	asagi::Grid::Error error;
	
	m_inputFile = new NetCdf(filename, getMPIRank());
	if ((error = m_inputFile->open(m_variableName.c_str()))
		!= asagi::Grid::SUCCESS)
		return error;
	
	m_dimX = m_inputFile->getXDim();
	m_dimY = m_inputFile->getYDim();
	m_dimZ = m_inputFile->getZDim();
	
	// A block size large than the dimension does not make any sense
	if (m_dimX < m_blockSizeX) {
		dbgDebug(getMPIRank()) << "Shrinking x block size to" << m_dimX;
		m_blockSizeX = m_dimX;
	}
	if (m_dimY < m_blockSizeY) {
		dbgDebug(getMPIRank()) << "Shrinking y block size to" << m_dimY;
		m_blockSizeY = m_dimY;
	}
	if (m_dimZ < m_blockSizeZ) {
		dbgDebug(getMPIRank()) << "Shrinking z block size to" << m_dimZ;
		m_blockSizeZ = m_dimZ;
	}
	
	offsetX = m_inputFile->getXOffset();
	offsetY = m_inputFile->getYOffset();
	offsetZ = m_inputFile->getZOffset();
	
	scalingX = m_inputFile->getXScaling();
	scalingY = m_inputFile->getYScaling();
	scalingZ = m_inputFile->getZScaling();
	
	scalingInvX = getInvScaling(scalingX);
	scalingInvY = getInvScaling(scalingY);
	scalingInvZ = getInvScaling(scalingZ);
	
	// Integer way of rounding up
	blocksX = (m_dimX + m_blockSizeX - 1) / m_blockSizeX;
	blocksY = (m_dimY + m_blockSizeY - 1) / m_blockSizeY;
	blocksZ = (m_dimZ + m_blockSizeZ - 1) / m_blockSizeZ;
	
	if ((error = getType().check(*m_inputFile)) != asagi::Grid::SUCCESS)
		return error;
	
	return init();
}

double Grid::getXMin()
{
	if (isinf(scalingX))
		return -std::numeric_limits<double>::infinity();
	
	if (m_container.getValuePos() == GridContainer::CELL_CENTERED)
		return offsetX + std::min(scalingX * (0. - 0.5),
			scalingX * (m_dimX - 1 - 0.5));
	
	return offsetX + std::min(0., (m_dimX - 1) * scalingX);
}

double Grid::getYMin()
{
	if (isinf(scalingY))
		return -std::numeric_limits<double>::infinity();
	
	if (m_container.getValuePos() == GridContainer::CELL_CENTERED)
		return offsetY + std::min(scalingY * (0. - 0.5),
			scalingY * (m_dimY - 1 - 0.5));
	
	return offsetY + std::min(0., (m_dimY - 1) * scalingY);
}

double Grid::getZMin()
{
	if (isinf(scalingZ))
		return -std::numeric_limits<double>::infinity();
	
	if (m_container.getValuePos() == GridContainer::CELL_CENTERED)
		return offsetZ + std::min(scalingZ * (0. - 0.5),
			scalingZ * (m_dimZ - 1 - 0.5));
	
	return offsetZ + std::min(0., (m_dimZ - 1) * scalingZ);
}

double Grid::getXMax()
{
	if (isinf(scalingX))
		return std::numeric_limits<double>::infinity();
	
	if (m_container.getValuePos() == GridContainer::CELL_CENTERED)
		return offsetX + std::max(scalingX * (0. - 0.5),
			scalingX * (m_dimX - 1 + 0.5));
	
	return offsetX + std::max(0., (m_dimX - 1) * scalingX);
}

double Grid::getYMax()
{
	if (isinf(scalingY))
		return std::numeric_limits<double>::infinity();
	
	if (m_container.getValuePos() == GridContainer::CELL_CENTERED)
		return offsetY + std::max(scalingY * (0. - 0.5),
			scalingY * (m_dimY - 1 + 0.5));
	
	return offsetY + std::max(0., (m_dimY - 1) * scalingY);
}

double Grid::getZMax()
{
	if (isinf(scalingZ))
		return std::numeric_limits<double>::infinity();
	
	if (m_container.getValuePos() == GridContainer::CELL_CENTERED)
		return offsetZ + std::max(scalingZ * (0. - 0.5),
			scalingZ * (m_dimZ - 1 + 0.5));
	
	return offsetZ + std::max(0., (m_dimZ - 1) * scalingZ);
}

char Grid::getByte(double x, double y, double z)
{
	char buf;
	getAt(&buf, &types::Type::convertByte, x, y, z);
	
	return buf;
}

int Grid::getInt(double x, double y, double z)
{
	int buf;
	getAt(&buf, &types::Type::convertInt, x, y, z);

	return buf;
}

long Grid::getLong(double x, double y, double z)
{
	long buf;
	getAt(&buf, &types::Type::convertLong, x, y, z);

	return buf;
}

float Grid::getFloat(double x, double y, double z)
{
	float buf;
	getAt(&buf, &types::Type::convertFloat, x, y, z);

	return buf;
}

double Grid::getDouble(double x, double y, double z)
{
	double buf;
	getAt(&buf, &types::Type::convertDouble, x, y, z);

	return buf;
}

void Grid::getBuf(void* buf, double x, double y, double z)
{
	getAt(buf, &types::Type::convertBuffer, x, y, z);
}

bool Grid::exportPng(const char* filename)
{
#ifdef PNG_ENABLED
	float min, max, value;
	unsigned char red, green, blue;
	
	min = max = getAtFloat(0, 0);
	for (unsigned long i = 0; i < m_dimX; i++) {
		for (unsigned long j = 0; j < m_dimY; j++) {
			value = getAtFloat(i, j);
			if (value < min)
				min = value;
			if (value > max)
				max = value;
		}
	}
	
	Png png(m_dimX, m_dimY);
	if (!png.create(filename))
		return false;
	
	for (unsigned long i = 0; i < m_dimX; i++) {
		for (unsigned long j = 0; j < m_dimY; j++) {
			// do some magic here
			h2rgb((getAtFloat(i, j) - min) / (max - min) * 2 / 3, red, green, blue);
			png.write(i, m_dimY - j - 1, red, green, blue);
		}
	}
	
	png.close();
	
	return true;
#else // PNG_ENABLED
	// TODO generate a warning or something like this
	return false;
#endif // PNG_ENABLED
}

void Grid::getAt(void* buf, types::Type::converter_t converter,
	double x, double y, double z)
{
	x = round((x - offsetX) * scalingInvX);
	y = round((y - offsetY) * scalingInvY);
	z = round((z - offsetZ) * scalingInvZ);

	assert(x >= 0 && x < getXDim()
		&& y >= 0 && y < getYDim()
		&& z >= 0 && z < getZDim());

	getAt(buf, converter, static_cast<unsigned long>(x),
		static_cast<unsigned long>(y), static_cast<unsigned long>(z));
}

float Grid::getAtFloat(unsigned long x, unsigned long y)
{
	float buf;
	
	getAt(&buf, &types::Type::convertFloat, x, y);
	
	return buf;
}

/**
 * @return The number of blocks we should store on this node
 */
unsigned long Grid::getBlocksPerNode()
{
	return BLOCKS_PER_NODE;
}

void Grid::h2rgb(float h, unsigned char &red, unsigned char &green,
	unsigned char &blue)
{
	// h from 0..1
	
	h *= 6;
	float x = fmod(h, 2) - 1;
	if (x < 0)
		x *= -1;
	x = 1 - x;
	
	// <= checks are not 100% correct, it should be <
	// but it solves the "larges-value" issue
	if (h <= 1) {
		red = 255;
		green = x * 255;
		blue = 0;
		return;
	}
	if (h <= 2) {
		red = x * 255;
		green = 255;
		blue = 0;
		return;
	}
	if (h <= 3) {
		red = 0;
		green = 255;
		blue = x * 255;
		return;
	}
	if (h <= 4) {
		red = 0;
		green = x * 255;
		blue = 255;
		return;
	}
	if (h <= 5) {
		red = x * 255;
		green = 0;
		blue = 255;
	}
	// h < 6
	red = 255;
	green = 0;
	blue = x * 255;
}

/**
 * Calculates 1/scaling, except for scaling = 0 and scaling = inf. In this
 * case it returns 0
 */
double Grid::getInvScaling(double scaling)
{
	if ((scaling == 0) || isinf(scaling))
		return 0;
	
	return 1/scaling;
}

double Grid::round(double value)
{
	return floor(value + 0.5);
}
