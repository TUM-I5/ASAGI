#include "grid.h"

#include "database_f95.h"

int f95grid_load(const char* filename)
{
	Grid* grid = new Grid();
	
	if (!grid->open(filename)) {
		delete grid;
		return NULL_INDEX;
	}
	
	return grid->c2f();
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

float f95grid_get_value(int grid_id, float x, float y)
{
	return Grid::f2c(grid_id)->get(x, y);
}

void f95grid_free(int grid_id)
{
	delete Grid::f2c(grid_id);
}