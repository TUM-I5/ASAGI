#include <asagi.h>

#include "arraygrid.h"
#include "basicgrid.h"

// Static c++ functions

bool asagi::Grid::init(MPI_Comm comm)
{
	return ::Grid::init(comm);
}

asagi::Grid* asagi::Grid::create(Type type)
{
	switch (type) {
		case BYTE:
			return new BasicGrid<char>();
		case INT:
			return new BasicGrid<int>();
		case LONG:
			return new BasicGrid<long>();
		case FLOAT:
			return new BasicGrid<float>();
		case DOUBLE:
			return new BasicGrid<double>();
		case BYTEARRAY:
			return new ArrayGrid<char>();
		case INTARRAY:
			return new ArrayGrid<int>();
		case LONGARRY:
			return new ArrayGrid<long>();
		case FLOATARRAY:
			return new ArrayGrid<float>();
		case DOUBLEARRAY:
			return new ArrayGrid<double>();
		default:
			;
	}
	
	return 0L; // should not happen
}

bool asagi::Grid::finalize()
{
	return ::Grid::finalize();
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