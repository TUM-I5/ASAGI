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
 * @copyright 2012-2013 Sebastian Rettenberger <rettenbs@in.tum.de>
 */

#include "level.h"

#include "io/netcdfreader.h"

#if 0
#include "constants.h"
#ifdef PNG_ENABLED
#include "io/pngwriter.h"
#endif

#include "types/basictype.h"

#include <cstdlib>
#include <cmath>
#include <limits>
#endif

grid::level::Level::Level()
	: m_comm(0L), m_numa(0L), m_type(0L),
	  m_inputFile(0L)
{
}

grid::level::Level::~Level()
{
	delete m_inputFile;
}


#if 0
/**
 * @param container The container, this grid belongs to
 * @param hint Optimization hints
 */
grid::Grid2::Grid2(const GridContainer &container,
	unsigned int hint)
	: m_container(container), m_variableName("z")
{
	m_inputFile = 0L;
	
	m_blockSize[0] = m_blockSize[1] = m_blockSize[2] = 0;
	
	m_blocksPerNode = -1;
	
	m_handSpread = -1;

	if (hint & asagi::Grid::HAS_TIME)
		m_timeDimension = -1;
	else
		m_timeDimension = -2;
}

grid::Grid2::~Grid2()
{
	delete m_inputFile;
}

/**
 * Accepts the following parameters:
 * @li @b variable-name
 * @li @b time-dimension
 * @li @b x-block-size
 * @li @b y-block-size
 * @li @b z-block-size
 * @li @b block-cache-size
 * @li @b cache-hand-spread
 * 
 * @see asagi::Grid::setParam(const char*, const char*, unsigned int)
 */
asagi::Grid::Error grid::Grid2::setParam(const char* name, const char* value)
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
			logWarning() << "Empty block cache size may lead to failures!";
		
		return asagi::Grid::SUCCESS;
	}
	
	if ((strcmp(name, "cache-hand-spread") == 0)
		|| (strcmp(name, "cache-hand-difference") == 0)) { // Obsolete name
		m_handSpread = atol(value);
		
		return asagi::Grid::SUCCESS;
	}
	
	return asagi::Grid::UNKNOWN_PARAM;
}
#endif

/**
 * Initialize the grid level
 */
asagi::Grid::Error grid::level::Level::_init(
		const mpi::MPIComm &comm,
		const numa::Numa &numa,
		const types::Type* type,
		const char* filename,
		const char* varname,
		grid::ValuePosition valuePos)
{
	asagi::Grid::Error err;

	m_comm = &comm;
	m_numa = &numa;
	m_type = type;

	double scaling[3];
	
	// Open NetCDF file
	m_inputFile = new io::NetCdfReader(filename, m_comm->rank());
	if ((err = m_inputFile->open(varname)) != asagi::Grid::SUCCESS)
		return err;
	
	m_dims = m_inputFile->dimensions();

	for (unsigned int i = 0; i < m_dims; i++) {
		// Get dimension size
		m_dim[i] = m_inputFile->getSize(i);
	
		// Get offset and scaling
		m_offset[i] = m_inputFile->getOffset(i);
	
		scaling[i] = m_inputFile->getScaling(i);
	}

#if 0
	// Set time dimension
	if (m_timeDimension == -1) {
		// Time grid, but time dimension not specified
		m_timeDimension = m_inputFile->getDimensions() - 1;
		logDebug(getMPIRank()) << "Assuming time dimension"
			<< DIMENSION_NAMES[m_timeDimension];
	}
	
	// Set block size in time dimension
	if ((m_timeDimension >= 0) && (m_blockSize[m_timeDimension] == 0)) {
		logDebug(getMPIRank()) << "Setting block size in time dimension"
			<< DIMENSION_NAMES[m_timeDimension] << "to 1";
		m_blockSize[m_timeDimension] = 1;
	}
#endif
	
	// Set default block size and calculate number of blocks
	for (unsigned int i = 0; i < m_dims; i++) {
#if 0
		if (m_blockSize[i] == 0)
			// Setting default block size, if not yet set
			m_blockSize[i] = 50;
		
		// A block size large than the dimension does not make any sense
		if (m_dim[i] < m_blockSize[i]) {
			logDebug(getMPIRank()) << "Shrinking" << DIMENSION_NAMES[i]
				<< "block size to" << m_dim[i];
			m_blockSize[i] = m_dim[i];
		}
		
		// Integer way of rounding up
		m_blocks[i] = (m_dim[i] + m_blockSize[i] - 1) / m_blockSize[i];
#endif
		
		m_scalingInv[i] = getInvScaling(scaling[i]);
		
		// Set min/max
		if (std::isinf(scaling[i])) {
			m_min[i] = -std::numeric_limits<double>::infinity();
			m_max[i] = std::numeric_limits<double>::infinity();
		} else {
			// Warning: min and max are inverted of scaling is negative
			double min = m_offset[i];
			double max = m_offset[i] + scaling[i] * (m_dim[i] - 1);

			if (valuePos == grid::CELL_CENTERED) {
				// Add half a cell on both ends
				min -= scaling[i] * (0.5 - NUMERIC_PRECISION);
				max += scaling[i] * (0.5 - NUMERIC_PRECISION);
			}

			m_min[i] = std::min(min, max);
			m_max[i] = std::max(min, max);
		}
	}
	
#if 0
	// Set default cache size
	if (m_blocksPerNode < 0)
		// Default value
		m_blocksPerNode = 80;
	
	// Init type
	if ((error = getType().check(*m_inputFile)) != asagi::Grid::SUCCESS)
		return error;
	
	// Init subclass
	error = init();
	
	if (!keepFileOpen()) {
		// input file no longer needed, so we close
		delete m_inputFile;
		m_inputFile = 0L;
	}
#endif
	
	return asagi::Grid::SUCCESS;
}

#if 0
/**
 * @return The value at (x,y,z) as a byte
 */
unsigned char grid::Grid2::getByte(double x, double y, double z)
{
	unsigned char buf;
	getAt(&buf, &types::Type::convertByte, x, y, z);
	
	return buf;
}

/**
 * @return The value at (x,y,z) as an integer
 */
int grid::Grid2::getInt(double x, double y, double z)
{
	int buf;
	getAt(&buf, &types::Type::convertInt, x, y, z);

	return buf;
}

/**
 * @return The value at (x,y,z) as a long
 */
long grid::Grid2::getLong(double x, double y, double z)
{
	long buf;
	getAt(&buf, &types::Type::convertLong, x, y, z);

	return buf;
}

/**
 * @return The value at (x,y,z) as a float
 */
float grid::Grid2::getFloat(double x, double y, double z)
{
	float buf;
	getAt(&buf, &types::Type::convertFloat, x, y, z);

	return buf;
}

/**
 * @return The value at (x,y,z) as a double
 */
double grid::Grid2::getDouble(double x, double y, double z)
{
	double buf;
	getAt(&buf, &types::Type::convertDouble, x, y, z);

	return buf;
}

/**
 * Copys the value at (x,y,z) into the buffer
 */
void grid::Grid2::getBuf(void* buf, double x, double y, double z)
{
	getAt(buf, &types::Type::convertBuffer, x, y, z);
}

/**
 * @see asagi::Grid::exportPng()
 */
bool grid::Grid2::exportPng(const char* filename)
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
	
	io::PngWriter png(getXDim(), getYDim());
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

/**
 * Converts the coordinates to indices and writes the value at the position
 * into the buffer
 */
void grid::Grid2::getAt(void* buf, types::Type::converter_t converter,
	double x, double y, double z)
{
	x = round((x - m_offset[0]) * m_scalingInv[0]);
	y = round((y - m_offset[1]) * m_scalingInv[1]);
	z = round((z - m_offset[2]) * m_scalingInv[2]);

	assert(x >= 0 && x < getXDim()
		&& y >= 0 && y < getYDim()
		&& z >= 0 && z < getZDim());

	m_counter.inc(perf::Counter::ACCESS);

	getAt(buf, converter, static_cast<unsigned long>(x),
		static_cast<unsigned long>(y), static_cast<unsigned long>(z));
}

float grid::Grid2::getAtFloat(unsigned long x, unsigned long y)
{
	float buf;
	
	getAt(&buf, &types::Type::convertFloat, x, y);
	
	return buf;
}

/**
 * Calculates a nice rgb representation for a value between 0 and 1
 */
void grid::Grid2::h2rgb(float h, unsigned char &red, unsigned char &green,
	unsigned char &blue)
{
	// h from 0..1
	
	h *= 6;
	float x = fmod(h, 2) - 1;
	if (x < 0)
		x *= -1;
	x = 1 - x;
	
	// <= checks are not 100% correct, it should be <
	// but it solves the "largest-value" issue
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
#endif

/**
 * Calculates 1/scaling, except for scaling = 0 and scaling = inf. In this
 * case it returns 0
 */
double grid::level::Level::getInvScaling(double scaling)
{
	if ((scaling == 0) || isinf(scaling))
		return 0;
	
	return 1/scaling;
}

/**
 * Implementation for round-to-nearest
 */
double grid::level::Level::round(double value)
{
	return floor(value + 0.5);
}
