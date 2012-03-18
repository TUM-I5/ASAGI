#include "grid.h"
#include "gridcontainer.h"

#include <math.h>
#include <limits>

#ifdef PNG_ENABLED
#include "io/png.h"
#endif

#include "types/basictype.h"
#include "debug/dbg.h"

#include <cstdlib>

using namespace io;

Grid::Grid(GridContainer &container, unsigned int hint)
	: m_container(container), m_variableName("z")
{
	m_inputFile = 0L;
	
	m_blockSize[0] = m_blockSize[1] = m_blockSize[2] = 0;
	
	m_blocksPerNode = -1;
	
	if (hint & asagi::HAS_TIME)
		m_timeDimension = -1;
	else
		m_timeDimension = -2;
}

Grid::~Grid()
{
	delete m_inputFile;
}

/**
 * Accpets the following parameters:
 * @li @b variable-name
 * @li @b time-dimension
 * @li @b x-block-size
 * @li @b y-block-size
 * @li @b z-block-size
 * @li @b block-cache-size
 * 
 * @see asagi::Grid::setParam(const char*, const char*, unsigned int)
 */
asagi::Grid::Error Grid::setParam(const char* name, const char* value)
{
	long blockSize;
	
	if (strcmp(name, "variable-name") == 0) {
		m_variableName = value;
		return asagi::Grid::SUCCESS;
	}
	
	if (strcmp(name, "time-dimension") == 0) {
		if (m_timeDimension <= -2)
			// HAS_TIME hint was not specified
			//-> ignore this variable
			return asagi::Grid::SUCCESS;
		
		// Value should be x, y or z
		for (signed char i = 0; i < 3; i++) {
			if (strcmp(value, DIMENSION_NAMES[i]) == 0) {
				m_timeDimension = i;
				return asagi::Grid::SUCCESS;
			}
		}
		
		return asagi::Grid::INVALID_VALUE;
	}
	
	if (strcmp(&name[1], "-block-size") == 0) {
		// Check for [xyz]-block-size
		
		for (signed char i = 0; i < 3; i++) {
			if (name[0] == DIMENSION_NAMES[i][0]) {
				blockSize = atol(value);
				
				if (blockSize <= 0)
					return asagi::Grid::INVALID_VALUE;
				
				m_blockSize[i] = blockSize;
				return asagi::Grid::SUCCESS;
			}
		}
	}
	
	if (strcmp(name, "block-cache-size") == 0) {
		m_blocksPerNode = atol(value);
		
		if (m_blocksPerNode < 0)
			// We set a correct value later
			return asagi::Grid::INVALID_VALUE;
		
		if ((m_blocksPerNode == 0) && (getMPISize() > 1))
			dbgDebug() << "Warning: empty block cache size may lead to failures!";
		
		return asagi::Grid::SUCCESS;
	}
	
	return asagi::Grid::UNKNOWN_PARAM;
}

asagi::Grid::Error Grid::open(const char* filename)
{
	asagi::Grid::Error error;
	
	// Open NetCDF file
	m_inputFile = new NetCdf(filename, getMPIRank());
	if ((error = m_inputFile->open(m_variableName.c_str()))
		!= asagi::Grid::SUCCESS)
		return error;
	
	// Get dimension size
	m_dim[0] = m_inputFile->getXDim();
	m_dim[1] = m_inputFile->getYDim();
	m_dim[2] = m_inputFile->getZDim();
	
	// Set time dimension
	if (m_timeDimension == -1) {
		// Time grid, but time dimension not sepecified
		m_timeDimension = m_inputFile->getDimensions() - 1;
		dbgDebug(getMPIRank()) << "Assuming time dimension"
			<< DIMENSION_NAMES[m_timeDimension];
	}
	
	// Set block size in time dimension
	if ((m_timeDimension >= 0) && (m_blockSize[m_timeDimension] == 0)) {
		dbgDebug(getMPIRank()) << "Setting block size in time dimension"
			<< DIMENSION_NAMES[m_timeDimension] << "to 1";
		m_blockSize[m_timeDimension] = 1;
	}
	
	// Set default block size and calculate number of blocks
#ifdef __INTEL_COMPILER
	#pragma unroll(3)
#endif // __INTEL_COMPILER
	for (unsigned char i = 0; i < 3; i++) {
		if (m_blockSize[i] == 0)
			// Setting default block size, if not yet set
			m_blockSize[i] = 50;
		
		// A block size large than the dimension does not make any sense
		if (m_dim[i] < m_blockSize[i]) {
			dbgDebug(getMPIRank()) << "Shrinking" << DIMENSION_NAMES[i]
				<< "block size to" << m_dim[i];
			m_blockSize[i] = m_dim[i];
		}
		
		// Integer way of rounding up
		blocks[i] = (m_dim[i] + m_blockSize[i] - 1) / m_blockSize[i];
	}
	
	// Set default cache size
	if (m_blocksPerNode < 0)
		// Default value
		m_blocksPerNode = 80;
	
	// Get values for min/max
	offsetX = m_inputFile->getXOffset();
	offsetY = m_inputFile->getYOffset();
	offsetZ = m_inputFile->getZOffset();
	
	scalingX = m_inputFile->getXScaling();
	scalingY = m_inputFile->getYScaling();
	scalingZ = m_inputFile->getZScaling();
	
	// Set scaling for coordinate -> index mapping
	scalingInvX = getInvScaling(scalingX);
	scalingInvY = getInvScaling(scalingY);
	scalingInvZ = getInvScaling(scalingZ);
	
	// Init type
	if ((error = getType().check(*m_inputFile)) != asagi::Grid::SUCCESS)
		return error;
	
	// Init subclass
	return init();
}

double Grid::getXMin()
{
	if (isinf(scalingX))
		return -std::numeric_limits<double>::infinity();
	
	if (m_container.getValuePos() == GridContainer::CELL_CENTERED)
		return offsetX + std::min(scalingX * (0. - 0.5),
			scalingX * (getXDim() - 1 - 0.5));
	
	return offsetX + std::min(0., (getXDim() - 1) * scalingX);
}

double Grid::getYMin()
{
	if (isinf(scalingY))
		return -std::numeric_limits<double>::infinity();
	
	if (m_container.getValuePos() == GridContainer::CELL_CENTERED)
		return offsetY + std::min(scalingY * (0. - 0.5),
			scalingY * (getYDim() - 1 - 0.5));
	
	return offsetY + std::min(0., (getYDim() - 1) * scalingY);
}

double Grid::getZMin()
{
	if (isinf(scalingZ))
		return -std::numeric_limits<double>::infinity();
	
	if (m_container.getValuePos() == GridContainer::CELL_CENTERED)
		return offsetZ + std::min(scalingZ * (0. - 0.5),
			scalingZ * (getZDim() - 1 - 0.5));
	
	return offsetZ + std::min(0., (getZDim() - 1) * scalingZ);
}

double Grid::getXMax()
{
	if (isinf(scalingX))
		return std::numeric_limits<double>::infinity();
	
	if (m_container.getValuePos() == GridContainer::CELL_CENTERED)
		return offsetX + std::max(scalingX * (0. - 0.5),
			scalingX * (getXDim() - 1 + 0.5));
	
	return offsetX + std::max(0., (getXDim() - 1) * scalingX);
}

double Grid::getYMax()
{
	if (isinf(scalingY))
		return std::numeric_limits<double>::infinity();
	
	if (m_container.getValuePos() == GridContainer::CELL_CENTERED)
		return offsetY + std::max(scalingY * (0. - 0.5),
			scalingY * (getYDim() - 1 + 0.5));
	
	return offsetY + std::max(0., (getYDim() - 1) * scalingY);
}

double Grid::getZMax()
{
	if (isinf(scalingZ))
		return std::numeric_limits<double>::infinity();
	
	if (m_container.getValuePos() == GridContainer::CELL_CENTERED)
		return offsetZ + std::max(scalingZ * (0. - 0.5),
			scalingZ * (getZDim() - 1 + 0.5));
	
	return offsetZ + std::max(0., (getZDim() - 1) * scalingZ);
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
	for (unsigned long i = 0; i < getXDim(); i++) {
		for (unsigned long j = 0; j < getYDim(); j++) {
			value = getAtFloat(i, j);
			if (value < min)
				min = value;
			if (value > max)
				max = value;
		}
	}
	
	Png png(getXDim(), getYDim());
	if (!png.create(filename))
		return false;
	
	for (unsigned long i = 0; i < getXDim(); i++) {
		for (unsigned long j = 0; j < getYDim(); j++) {
			// do some magic here
			h2rgb((getAtFloat(i, j) - min) / (max - min) * 2 / 3, red, green, blue);
			png.write(i, getYDim() - j - 1, red, green, blue);
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

const char* Grid::DIMENSION_NAMES[] = {"x", "y", "z"};

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
