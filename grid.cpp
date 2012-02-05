#include <math.h>

#ifdef WITH_PNG
#include "io/png.h"
#endif

#include "types/basictype.h"

#include "grid.h"

// TODO
#define BLOCK_SIZE_X 50
#define BLOCK_SIZE_Y 50
#define BLOCKS_PER_NODE 80

using namespace io;

Grid::Grid(Type type)
{
	// Prepare for fortran <-> c translation
	id = pointers.add(this);
	
	file = 0L;
	
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
	delete file;
	
	delete m_type;
	
	// Remove from fortran <-> c translation
	pointers.remove(id);
}

bool Grid::open(const char* filename)
{
	MPI_Comm_rank(communicator, &m_mpiRank);
	MPI_Comm_size(communicator, &m_mpiSize);
	
	file = new NetCdf(filename);
	if (!file->open())
		return false;
	
	dimX = file->getXDim();
	dimY = file->getYDim();
	
	offsetX = file->getXOffset();
	offsetY = file->getYOffset();
	
	scalingX = file->getXScaling();
	scalingY = file->getYScaling();
	
	// Integer way of rounding up
	blocksX = (dimX + BLOCK_SIZE_X - 1) / BLOCK_SIZE_X;
	blocksY = (dimY + BLOCK_SIZE_Y - 1) / BLOCK_SIZE_Y;
	
	if (!m_type->check(*file))
		return false;
	
	return init();
}

float Grid::getXMin()
{
	return offsetX + std::min(0.f, dimX * scalingX);
}

float Grid::getYMin()
{
	return offsetY + std::min(0.f, dimY * scalingY);
}

float Grid::getXMax()
{
	return offsetX + std::max(0.f, dimX * scalingX);
}

float Grid::getYMax()
{
	return offsetY + std::max(0.f, dimY * scalingY);
}

unsigned int Grid::getVarSize()
{
	return m_type->getSize();
}

char Grid::getByte(float x, float y)
{
	return m_type->getByte(getAt(x, y));
}

int Grid::getInt(float x, float y)
{
	return m_type->getInt(getAt(x, y));
}

long Grid::getLong(float x, float y)
{
	return m_type->getLong(getAt(x, y));
}

float Grid::getFloat(float x, float y)
{
	return m_type->getFloat(getAt(x, y));
}

double Grid::getDouble(float x, float y)
{
	return m_type->getDouble(getAt(x, y));
}

void Grid::getBuf(float x, float y, void* buf)
{
	memcpy(buf, getAt(x, y), m_type->getSize());
}
bool Grid::exportPng(const char* filename)
{
#ifdef WITH_PNG
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
#else // WITH_PNG
	// TODO generate a warning or something like this
	return false;
#endif // WITH_PNG
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

void* Grid::getAt(float x, float y)
{
	x = round((x - offsetX) / scalingX);
	y = round((y - offsetY) / scalingY);
	
	assert(x >= 0 && x < getXDim()
		&& y >= 0 && y < getYDim());
	
	return getAt(static_cast<unsigned long>(x),
		static_cast<unsigned long>(y));
}

float Grid::getAtFloat(unsigned long x, unsigned long y)
{
	return m_type->getFloat(getAt(x, y));
}

long unsigned Grid::getXDim()
{
	return dimX;
}

long unsigned Grid::getYDim()
{
	return dimY;
}

float Grid::getXOffset()
{
	return offsetX;
}

float Grid::getYOffset()
{
	return offsetY;
}

float Grid::getXScaling()
{
	return scalingX;
}

float Grid::getYScaling()
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

MPI_Comm Grid::communicator;

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

float Grid::round(float value)
{
	return floor(value + 0.5);
}

bool Grid::init(MPI_Comm comm)
{
	if (MPI_Comm_dup(comm, &communicator) != MPI_SUCCESS)
		return false;
		
	return true;
}

bool Grid::finalize()
{
	if (MPI_Comm_free(&communicator) != MPI_SUCCESS)
		return false;
	
	return true;
}

Grid* Grid::f2c(int i)
{
	return pointers.get(i);
}