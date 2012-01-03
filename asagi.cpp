#include <asagi.h>

#include "typedgrid.h"

// Static c++ functions
asagi::Grid* asagi::Grid::create(Type type)
{
	switch (type) {
		case BYTE:
			return new TypedGrid<char, char>();
		case INT:
			return new TypedGrid<int, int>();
		case LONG:
			return new TypedGrid<long, long>();
		case FLOAT:
			return new TypedGrid<float, float>();
		case DOUBLE:
			return new TypedGrid<double, double>();
		case BYTEARRAY:
			return new TypedGrid<char, char*>();
		default:
			;
	}
	
	return 0L; // should not happen
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

float grid_get_float(grid_handle* handle, float x, float y)
{
	return handle->getFloat(x, y);
}

void grid_free(grid_handle* handle)
{
	delete handle;
}