#include "database_f77.h"
#include "grid.h"
#include "fortran/string.h"

#define GRID_GENERATE_F77_BINDINGS(lower_case, \
		upper_case, \
		signature, \
		params) \
	void F##upper_case signature { f##lower_case params; } \
	void f##lower_case##_ signature { f##lower_case params; } \
	void f##lower_case##__ signature { f##lower_case params; }

using namespace fortran;

GRID_GENERATE_F77_BINDINGS(grid_load, GRID_LOAD,
	(char *filename, Fint* grid_id, int filename_len), (filename, grid_id, filename_len))

void fgrid_load(char* filename, int* grid_id, int filename_len)
{
	char* c_filename;
	Grid* grid;
	
	if (!String::f2c(filename, filename_len, &c_filename)) {
		*grid_id = NULL_INDEX;
		return;
	}
	
	grid = new Grid();
	if (!grid->open(c_filename)) {
		*grid_id = NULL_INDEX;
		delete c_filename;
		return;
	}
	
	*grid_id = grid->c2f();
	delete c_filename;
}

GRID_GENERATE_F77_BINDINGS(grid_x, GRID_X,
	(Fint* grid_id, Fint* x), (grid_id, x))

void fgrid_x(Fint* grid_id, Fint* x)
{
	*x = Grid::f2c(*grid_id)->getXDim();
}

GRID_GENERATE_F77_BINDINGS(grid_y, GRID_Y,
	(Fint* grid_id, Fint* y), (grid_id, y))

void fgrid_y(Fint* grid_id, Fint* y)
{
	*y = Grid::f2c(*grid_id)->getYDim();
}

GRID_GENERATE_F77_BINDINGS(grid_get_value, GRID_GET_VALUE,
	(Fint* grid_id, Fint* x, Fint* y, Freal* value), (grid_id, x, y, value))

void fgrid_get_value(Fint* grid_id, Fint* x, Fint* y, Freal* value)
{
	*value = Grid::f2c(*grid_id)->get(*x, *y);
}