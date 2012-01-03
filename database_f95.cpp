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