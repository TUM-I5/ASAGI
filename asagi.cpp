#include <asagi.h>

#include "simplegridcontainer.h"
#include "adaptivegridcontainer.h"

// Empty destructor
asagi::Grid::~Grid()
{
}

// Static c++ functions
asagi::Grid* asagi::Grid::create(Type type, unsigned int hint,
	unsigned int levels)
{
	if (hint & asagi::ADAPTIVE)
		return new AdaptiveGridContainer(type, false, hint, levels);
	return new SimpleGridContainer(type, false, hint, levels);
}

asagi::Grid* asagi::Grid::createArray(Type basicType, unsigned int hint,
	unsigned int levels)
{
	if (hint & asagi::ADAPTIVE)
		return new AdaptiveGridContainer(basicType, false, hint, levels);
	return new SimpleGridContainer(basicType, true, hint, levels);
}

// C interfae

// Init functions

grid_handle* grid_create(grid_type type, unsigned int hint, unsigned int levels)
{
	return asagi::Grid::create(type, hint, levels);
}

grid_handle* grid_create_array(grid_type basic_type, unsigned int hint, unsigned int levels)
{
	return asagi::Grid::createArray(basic_type, hint, levels);
}

#ifndef ASAGI_NOMPI
grid_error grid_set_comm(grid_handle* handle, MPI_Comm comm)
{
	return handle->setComm(comm);
}
#endif // ASAIG_NOMPI

grid_error grid_set_param(grid_handle* handle, const char* name,
	const char* value, unsigned int level)
{
	return handle->setParam(name, value, level);
}

grid_error grid_open(grid_handle* handle, const char* filename,
	unsigned int level)
{
	return handle->open(filename, level);
}

// Min/Max functions

double grid_min_x(grid_handle* handle)
{
	return handle->getXMin();
}
double grid_min_y(grid_handle* handle)
{
	return handle->getYMin();
}
double grid_max_x(grid_handle* handle)
{
	return handle->getXMax();
}
double grid_max_y(grid_handle* handle)
{
	return handle->getYMax();
}

// 1d functions

char grid_get_byte_1d(grid_handle* handle, double x, unsigned int level)
{
	return handle->getByte1D(x, level);
}
int grid_get_int_1d(grid_handle* handle, double x, unsigned int level)
{
	return handle->getInt1D(x, level);
}
long grid_get_long_1d(grid_handle* handle, double x, unsigned int level)
{
	return handle->getLong1D(x, level);
}
float grid_get_float_1d(grid_handle* handle, double x, unsigned int level)
{
	return handle->getFloat1D(x, level);
}
double grid_get_double_1d(grid_handle* handle, double x, unsigned int level)
{
	return handle->getDouble1D(x, level);
}
void grid_get_buf_1d(grid_handle* handle, void* buf, double x,
	unsigned int level)
{
	handle->getBuf1D(buf, x, level);
}

// 2d functions

char grid_get_byte_2d(grid_handle* handle, double x, double y,
	unsigned int level)
{
	return handle->getByte2D(x, y, level);
}
int grid_get_int_2d(grid_handle* handle, double x, double y, unsigned int level)
{
	return handle->getInt2D(x, y, level);
}
long grid_get_long_2d(grid_handle* handle, double x, double y,
	unsigned int level)
{
	return handle->getLong2D(x, y, level);
}
float grid_get_float_2d(grid_handle* handle, double x, double y,
	unsigned int level)
{
	return handle->getFloat2D(x, y, level);
}
double grid_get_double_2d(grid_handle* handle, double x, double y,
	unsigned int level)
{
	return handle->getDouble2D(x, y, level);
}
void grid_get_buf_2d(grid_handle* handle, void* buf, double x, double y,
	unsigned int level)
{
	handle->getBuf2D(buf, x, y, level);
}

// 3d functions

char grid_get_byte_3d(grid_handle* handle, double x, double y, double z,
	unsigned int level)
{
	return handle->getByte3D(x, y, z, level);
}
int grid_get_int_3d(grid_handle* handle, double x, double y, double z,
	unsigned int level)
{
	return handle->getInt3D(x, y, z, level);
}
long grid_get_long_3d(grid_handle* handle, double x, double y, double z,
	unsigned int level)
{
	return handle->getLong3D(x, y, z, level);
}
float grid_get_float_3d(grid_handle* handle, double x, double y, double z,
	unsigned int level)
{
	return handle->getFloat3D(x, y, z, level);
}
double grid_get_double_3d(grid_handle* handle, double x, double y, double z,
	unsigned int level)
{
	return handle->getDouble3D(x, y, z, level);
}
void grid_get_buf_3d(grid_handle* handle, void* buf, double x, double y, double z,
	unsigned int level)
{
	handle->getBuf3D(buf, x, y, z, level);
}

// destructor

void grid_close(grid_handle* handle)
{
	delete handle;
}
