#include "gridcontainer.h"

#include "asagi_f90.h"

// Init functions

int f90grid_create(grid_type type, int hint, int levels)
{
	return static_cast<GridContainer*>(
		asagi::Grid::create(type, hint, levels))->c2f();
}

int f90grid_create_array(grid_type basic_type, int hint, int levels)
{
	return static_cast<GridContainer*>(
		asagi::Grid::createArray(basic_type, hint, levels))->c2f();
}

grid_error f90grid_set_comm(int grid_id, int comm)
{
	// MPI_Comm_f2c expects an MPI_Fint, however iso_c_bindings
	// already converts this parameter into c integer
	return GridContainer::f2c(grid_id)->setComm(MPI_Comm_f2c(comm));
}

grid_error f90grid_set_param(int grid_id, const char* name,
	const char* value, int level)
{
	return GridContainer::f2c(grid_id)->setParam(name, value, level);
}

grid_error f90grid_open(int grid_id, const char* filename,
	int level)
{
	return GridContainer::f2c(grid_id)->open(filename, level);
}

// Min/Max functions

double f90grid_min_x(int grid_id)
{
	return GridContainer::f2c(grid_id)->getXMin();
}
double f90grid_min_y(int grid_id)
{
	return GridContainer::f2c(grid_id)->getYMin();
}
double f90grid_max_x(int grid_id)
{
	return GridContainer::f2c(grid_id)->getXMax();
}
double f90grid_max_y(int grid_id)
{
	return GridContainer::f2c(grid_id)->getYMax();
}

// 1d functions

char f90grid_get_byte_1d(int grid_id, double x, int level)
{
	return GridContainer::f2c(grid_id)->getByte1D(x, level);
}
int f90grid_get_int_1d(int grid_id, double x, int level)
{
	return GridContainer::f2c(grid_id)->getInt1D(x, level);
}
long f90grid_get_long_1d(int grid_id, double x, int level)
{
	return GridContainer::f2c(grid_id)->getLong1D(x, level);
}
float f90grid_get_float_1d(int grid_id, double x, int level)
{
	return GridContainer::f2c(grid_id)->getFloat1D(x, level);
}
double f90grid_get_double_1d(int grid_id, double x, int level)
{
	return GridContainer::f2c(grid_id)->getDouble1D(x, level);
}
void f90grid_get_buf_1d(int grid_id, void* buf, double x,
	int level)
{
	GridContainer::f2c(grid_id)->getBuf1D(buf, x, level);
}

// 2d functions

char f90grid_get_byte_2d(int grid_id, double x, double y,
	int level)
{
	return GridContainer::f2c(grid_id)->getByte2D(x, y, level);
}
int f90grid_get_int_2d(int grid_id, double x, double y, int level)
{
	return GridContainer::f2c(grid_id)->getInt2D(x, y, level);
}
long f90grid_get_long_2d(int grid_id, double x, double y,
	int level)
{
	return GridContainer::f2c(grid_id)->getLong2D(x, y, level);
}
float f90grid_get_float_2d(int grid_id, double x, double y,
	int level)
{
	return GridContainer::f2c(grid_id)->getFloat2D(x, y, level);
}
double f90grid_get_double_2d(int grid_id, double x, double y,
	int level)
{
	return GridContainer::f2c(grid_id)->getDouble2D(x, y, level);
}
void f90grid_get_buf_2d(int grid_id, void* buf, double x, double y,
	int level)
{
	GridContainer::f2c(grid_id)->getBuf2D(buf, x, y, level);
}

// 3d functions

char f90grid_get_byte_3d(int grid_id, double x, double y, double z,
	int level)
{
	return GridContainer::f2c(grid_id)->getByte3D(x, y, z, level);
}
int f90grid_get_int_3d(int grid_id, double x, double y, double z,
	int level)
{
	return GridContainer::f2c(grid_id)->getInt3D(x, y, z, level);
}
long f90grid_get_long_3d(int grid_id, double x, double y, double z,
	int level)
{
	return GridContainer::f2c(grid_id)->getLong3D(x, y, z, level);
}
float f90grid_get_float_3d(int grid_id, double x, double y, double z,
	int level)
{
	return GridContainer::f2c(grid_id)->getFloat3D(x, y, z, level);
}
double f90grid_get_double_3d(int grid_id, double x, double y, double z,
	int level)
{
	return GridContainer::f2c(grid_id)->getDouble3D(x, y, z, level);
}
void f90grid_get_buf_3d(int grid_id, void* buf, double x, double y, double z,
	int level)
{
	GridContainer::f2c(grid_id)->getBuf3D(buf, x, y, z, level);
}

// destructor

void f90grid_close(int grid_id)
{
	delete GridContainer::f2c(grid_id);
}
