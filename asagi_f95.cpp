#include "grid.h"

#include "asagi_f95.h"

int f95grid_create(grid_type type)
{
	return static_cast<Grid*>(asagi::Grid::create(type))->c2f();
}

bool f95grid_open(int grid_id, const char* filename, int comm)
{
	// MPI_Comm_f2c expects an MPI_Fint, however iso_c_bindings
	// already converts this parameter into c integer
	return Grid::f2c(grid_id)->open(filename, MPI_Comm_f2c(comm));
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

char f95grid_get_byte(int grid_id, float x, float y)
{
	return Grid::f2c(grid_id)->getByte(x, y);
}

int f95grid_get_int(int grid_id, float x, float y)
{
	return Grid::f2c(grid_id)->getInt(x, y);
}

long f95grid_get_long(int grid_id, float x, float y)
{
	return Grid::f2c(grid_id)->getLong(x, y);
}

float f95grid_get_float(int grid_id, float x, float y)
{
	return Grid::f2c(grid_id)->getFloat(x, y);
}

double f95grid_get_double(int grid_id, float x, float y)
{
	return Grid::f2c(grid_id)->getDouble(x, y);
}

void f95grid_get_buf(int grid_id, float x, float y, void* buf)
{
	return Grid::f2c(grid_id)->getBuf(x, y, buf);
}

void f95grid_free(int grid_id)
{
	delete Grid::f2c(grid_id);
}