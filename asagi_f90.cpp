#include "grid.h"

#include "asagi_f90.h"

int f90grid_create(grid_type type)
{
	return static_cast<Grid*>(asagi::Grid::create(type))->c2f();
}

bool f90grid_open(int grid_id, const char* filename, int comm)
{
	// MPI_Comm_f2c expects an MPI_Fint, however iso_c_bindings
	// already converts this parameter into c integer
	return Grid::f2c(grid_id)->open(filename, MPI_Comm_f2c(comm));
}

double f90grid_min_x(int grid_id)
{
	return Grid::f2c(grid_id)->getXMin();
}

double f90grid_min_y(int grid_id)
{
	return Grid::f2c(grid_id)->getYMin();
}

double f90grid_max_x(int grid_id)
{
	return Grid::f2c(grid_id)->getXMax();
}

double f90grid_max_y(int grid_id)
{
	return Grid::f2c(grid_id)->getYMax();
}

char f90grid_get_byte(int grid_id, double x, double y)
{
	return Grid::f2c(grid_id)->getByte(x, y);
}

int f90grid_get_int(int grid_id, double x, double y)
{
	return Grid::f2c(grid_id)->getInt(x, y);
}

long f90grid_get_long(int grid_id, double x, double y)
{
	return Grid::f2c(grid_id)->getLong(x, y);
}

float f90grid_get_float(int grid_id, double x, double y)
{
	return Grid::f2c(grid_id)->getFloat(x, y);
}

double f90grid_get_double(int grid_id, double x, double y)
{
	return Grid::f2c(grid_id)->getDouble(x, y);
}

void f90grid_get_buf(int grid_id, double x, double y, void* buf)
{
	return Grid::f2c(grid_id)->getBuf(x, y, buf);
}

void f90grid_free(int grid_id)
{
	delete Grid::f2c(grid_id);
}