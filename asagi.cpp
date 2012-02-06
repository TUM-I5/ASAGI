#include <asagi.h>

#include "grid.h"
#include "simplegrid.h"

// Empty destructor
asagi::Grid::~Grid()
{
}

// Static c++ functions
asagi::Grid* asagi::Grid::create(Type type)
{
	return new SimpleGrid(type);
}

// C interfae
grid_handle* grid_create(grid_type type)
{
	return asagi::Grid::create(type);
}

int grid_open(grid_handle* handle, const char* filename)
{
	return handle->open(filename);
}

float grid_min_x(grid_handle* handle)
{
	return handle->getXMin();
}

float grid_min_y(grid_handle* handle)
{
	return handle->getYMin();
}

float grid_max_x(grid_handle* handle)
{
	return handle->getXMax();
}

float grid_max_y(grid_handle* handle)
{
	return handle->getYMax();
}

char grid_get_byte(grid_handle* handle, float x, float y)
{
	return handle->getByte(x, y);
}

int grid_get_int(grid_handle* handle, float x, float y)
{
	return handle->getInt(x, y);
}

long grid_get_long(grid_handle* handle, float x, float y)
{
	return handle->getLong(x, y);
}

float grid_get_float(grid_handle* handle, float x, float y)
{
	return handle->getFloat(x, y);
}

double grid_get_double(grid_handle* handle, float x, float y)
{
	return handle->getDouble(x, y);
}

void grid_get_buf(grid_handle* handle, float x, float y, void* buf)
{
	handle->getBuf(x, y, buf);
}

void grid_free(grid_handle* handle)
{
	delete handle;
}