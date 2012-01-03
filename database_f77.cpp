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

GRID_GENERATE_F77_BINDINGS(grid_min_x, GRID_MIN_X,
	(Fint* grid_id, Freal* x), (grid_id, x))

void fgrid_min_x(Fint* grid_id, Freal* x)
{
	*x = Grid::f2c(*grid_id)->getXMin();
}

GRID_GENERATE_F77_BINDINGS(grid_min_y, GRID_MIN_Y,
	(Fint* grid_id, Freal* y), (grid_id, y))

void fgrid_min_y(Fint* grid_id, Freal* y)
{
	*y = Grid::f2c(*grid_id)->getYMin();
}

GRID_GENERATE_F77_BINDINGS(grid_max_x, GRID_MAX_X,
	(Fint* grid_id, Freal* x), (grid_id, x))

void fgrid_max_x(Fint* grid_id, Freal* x)
{
	*x = Grid::f2c(*grid_id)->getXMax();
}

GRID_GENERATE_F77_BINDINGS(grid_max_y, GRID_MAX_Y,
	(Fint* grid_id, Freal* y), (grid_id, y))

void fgrid_max_y(Fint* grid_id, Freal* y)
{
	*y = Grid::f2c(*grid_id)->getYMax();
}

GRID_GENERATE_F77_BINDINGS(grid_get_value, GRID_GET_VALUE,
	(Fint* grid_id, Freal* x, Freal* y, Freal* value), (grid_id, x, y, value))

void fgrid_get_value(Fint* grid_id, Freal* x, Freal* y, Freal* value)
{
	*value = Grid::f2c(*grid_id)->get(*x, *y);
}