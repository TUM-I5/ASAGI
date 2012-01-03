#include "grid.h"

Grid::Grid()
{
	// Prepare for fortran <-> c translation
	id = pointers.add(this);
	
	file = 0L;
}

Grid::~Grid()
{
	delete file;
	
	// Remove from fortran <-> c translation
	pointers.remove(id);
}

bool Grid::open(const char* filename)
{
	file = new io::NetCdf(filename);
	if (file->hasError()) {
		delete file;
		file = 0L;
		return false;
	}
	
	x = file->getXDim();
	y = file->getYDim();
	values = file->getAll();
	
	return true;
}

unsigned long Grid::getXDim()
{
	return x;
}

unsigned long Grid::getYDim()
{
	return y;
}

float Grid::get(unsigned long x, unsigned long y)
{
	// no range checking for x and y for performance reason
	
	if (file->isDimSwitched()) {
		// switch x and y
		x ^= y;
		y ^= x;
		x ^= y;
		
		x *= getXDim() + y;
	} else
		x *= getYDim() + y;
	
	return values[x];
}

void Grid::exportPNG(const char* filename)
{
}

int Grid::c2f()
{
	return id;
}

// Fortran <-> c translation array
fortran::PointerArray<Grid> Grid::pointers;

Grid* Grid::f2c(int i)
{
	return pointers.get(i);
}