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

float grid_get_value(grid_handle* handle, float x, float y)
{
	return handle->get(x, y);
}

void grid_free(grid_handle* handle)
{
	delete handle;
}