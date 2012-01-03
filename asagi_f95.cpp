#include "grid.h"

#include "asagi_f95.h"

int f95grid_create(grid_type type)
{
	return static_cast<Grid*>(asagi::Grid::create(type))->c2f();
}

bool f95grid_open(int grid_id, const char* filename)
{
	return Grid::f2c(grid_id)->open(filename);
}

float f95grid_min_x(int grid_id)
{
	return Grid::f2c(grid_id)->getXMin();
}

float f95grid_min_y(int grid_id)
{
	return Grid::f2c(grid_id)->getYMin();
}

float f95grid_max_x(int grid_id)
{
	return Grid::f2c(grid_id)->getXMax();
}

float f95grid_max_y(int grid_id)
{
	return Grid::f2c(grid_id)->getYMax();
}

float f95grid_get_float(int grid_id, float x, float y)
{
	return Grid::f2c(grid_id)->getFloat(x, y);
}

void f95grid_free(int grid_id)
{
	delete Grid::f2c(grid_id);
}