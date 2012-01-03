#include <database.h>

#include "grid.h"

grid::Grid* grid::Grid::create()
{
	return new ::Grid();
}

grid_handle* grid_load(const char* filename)
{
	Grid* grid = new Grid();
	
	if (!grid->open(filename)) {
		delete grid;
		return 0L;
	}
	
	return grid;
}

unsigned long grid_x(grid_handle* handle)
{
	return handle->getXDim();
}

unsigned long grid_y(grid_handle* handle)
{
	return handle->getYDim();
}

float grid_get_value(grid_handle* handle, unsigned long x, unsigned long y)
{
	return handle->get(x, y);
}