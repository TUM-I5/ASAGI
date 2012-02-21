#include <grid.h>

#include <math.h>

#ifdef PNG_ENABLED
#include "io/png.h"
#endif

#include "types/basictype.h"

// TODO
#define BLOCK_SIZE_X 50
#define BLOCK_SIZE_Y 50
#define BLOCKS_PER_NODE 80

using namespace io;

Grid::Grid(Type type)
{
	// Prepare for fortran <-> c translation
	id = pointers.add(this);
	
	m_inputFile = 0L;
	
	communicator = MPI_COMM_NULL;
	
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
	/*case BYTEARRAY:
		return new ArrayGrid<char>();
	case INTARRAY:
		return new ArrayGrid<int>();
	case LONGARRY:
		return new ArrayGrid<long>();
	case FLOATARRAY:
		return new ArrayGrid<float>();
	case DOUBLEARRAY:
		return new ArrayGrid<double>();*/
	default:
		m_type = 0L;
		assert(false);
	}
}

Grid::~Grid()
{
	delete m_inputFile;
	
	delete m_type;
	
	MPI_Comm_free(&communicator);
	
	// Remove from fortran <-> c translation
	pointers.remove(id);
}

bool Grid::open(const char* filename, MPI_Comm comm)
{
	if (MPI_Comm_dup(comm, &communicator) != MPI_SUCCESS)
		return false;
	
	MPI_Comm_rank(communicator, &m_mpiRank);
	MPI_Comm_size(communicator, &m_mpiSize);
	
	m_inputFile = new NetCdf(filename);
	if (!m_inputFile->open())
		return false;
	
	dimX = m_inputFile->getXDim();
	dimY = m_inputFile->getYDim();
	
	offsetX = m_inputFile->getXOffset();
	offsetY = m_inputFile->getYOffset();
	
	scalingX = m_inputFile->getXScaling();
	scalingY = m_inputFile->getYScaling();
	
	// Integer way of rounding up
	blocksX = (dimX + BLOCK_SIZE_X - 1) / BLOCK_SIZE_X;
	blocksY = (dimY + BLOCK_SIZE_Y - 1) / BLOCK_SIZE_Y;
	
	if (!m_type->check(*m_inputFile))
		return false;
	
	return init();
}

double Grid::getXMin()
{
	return offsetX + std::min(scalingX * (0. - 0.5), scalingX * (dimX - 0.5));
}

double Grid::getYMin()
{
	return offsetY + std::min(scalingY * (0. - 0.5), scalingY * (dimY - 0.5));
}

double Grid::getXMax()
{
	return offsetX + std::max(scalingX * (0. - 0.5), scalingX * (dimX - 0.5));
}

double Grid::getYMax()
{
	return offsetY + std::max(scalingY * (0. - 0.5), scalingY * (dimY - 0.5));
}

unsigned int Grid::getVarSize()
{
	return m_type->getSize();
}

char Grid::getByte(double x, double y)
{
	char buf;
	getAt(x, y, &buf, &types::Type::convertByte);
	
	return buf;
}

int Grid::getInt(double x, double y)
{
	int buf;
	getAt(x, y, &buf, &types::Type::convertInt);

	return buf;
}

long Grid::getLong(double x, double y)
{
	long buf;
	getAt(x, y, &buf, &types::Type::convertLong);

	return buf;
}

float Grid::getFloat(double x, double y)
{
	float buf;
	getAt(x, y, &buf, &types::Type::convertFloat);

	return buf;
}

double Grid::getDouble(double x, double y)
{
	double buf;
	getAt(x, y, &buf, &types::Type::convertDouble);

	return buf;
}

void Grid::getBuf(double x, double y, void* buf)
{
	getAt(x, y, buf, &types::Type::convertBuffer);
}

bool Grid::exportPng(const char* filename)
{
#ifdef PNG_ENABLED
	float min, max, value;
	unsigned char red, green, blue;
	
	min = max = getAtFloat(0, 0);
	for (unsigned long i = 0; i < dimX; i++) {
		for (unsigned long j = 0; j < dimY; j++) {
			value = getAtFloat(i, j);
			if (value < min)
				min = value;
			if (value > max)
				max = value;
		}
	}
	
	Png png(dimX, dimY);
	if (!png.create(filename))
		return false;
	
	for (unsigned long i = 0; i < dimX; i++) {
		for (unsigned long j = 0; j < dimY; j++) {
			// do some magic here
			h2rgb((getAtFloat(i, j) - min) / (max - min) * 2 / 3, red, green, blue);
			png.write(i, dimY - j - 1, red, green, blue);
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
 * Converts the C pointer of the grid to the Fortran identifier
 * 
 * @return The unique index of the grid
 */
int Grid::c2f()
{
	return id;
}

void Grid::getAt(double x, double y, void* buf,
	types::Type::converter_t converter)
{
	x = round((x - offsetX) / scalingX);
	y = round((y - offsetY) / scalingY);

	assert(x >= 0 && x < getXDim()
		&& y >= 0 && y < getYDim());

	getAt(static_cast<unsigned long>(x),
		static_cast<unsigned long>(y),
		buf, converter);
}

float Grid::getAtFloat(unsigned long x, unsigned long y)
{
	float buf;
	getAt(x, y, &buf, &types::Type::convertFloat);
	
	return buf;
}

MPI_Comm Grid::getMPICommunicator()
{
	return communicator;
}

int Grid::getMPIRank()
{
	return m_mpiRank;
}

int Grid::getMPISize()
{
	return m_mpiSize;
}

NetCdf* Grid::getInputFile()
{
	return m_inputFile;
}

types::Type* Grid::getType()
{
	return m_type;
}

long unsigned Grid::getXDim()
{
	return dimX;
}

long unsigned Grid::getYDim()
{
	return dimY;
}

double Grid::getXOffset()
{
	return offsetX;
}

double Grid::getYOffset()
{
	return offsetY;
}

double Grid::getXScaling()
{
	return scalingX;
}

double Grid::getYScaling()
{
	return scalingY;
}

/**
 * @return The number of blocks we should store on this node
 */
unsigned long Grid::getBlocksPerNode()
{
	return BLOCKS_PER_NODE;
}

/**
 * @return The number of values in x direction in each block
 */
unsigned long Grid::getXBlockSize()
{
	return BLOCK_SIZE_X;
}

/**
 * @return The number of values in y direction in each block
 */
unsigned long Grid::getYBlockSize()
{
	return BLOCK_SIZE_Y;
}

/**
 * @return The number of blocks in the grid
 */
unsigned long Grid::getBlockCount()
{
	return blocksX * blocksY;
}

/**
 * Calculates the position of <code>block</code> in the grid
 */
void Grid::getBlockPos(unsigned long block, unsigned long &x, unsigned long &y)
{
	x = block % blocksX;
	y = block / blocksX;
}

/**
 * @return The block that stores the value at (x, y)
 */
unsigned long Grid::getBlockByCoords(unsigned long x, unsigned long y)
{
	return (y / BLOCK_SIZE_Y) * blocksX + (x / BLOCK_SIZE_X);
}

// Fortran <-> c translation array
fortran::PointerArray<Grid> Grid::pointers;

void Grid::h2rgb(float h, unsigned char &red, unsigned char &green, unsigned char &blue)
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

double Grid::round(double value)
{
	return floor(value + 0.5);
}

Grid* Grid::f2c(int i)
{
	return pointers.get(i);
}
