#include "grid.h"

Grid::Grid()
{
	file = 0L;
}

Grid::~Grid()
{
	delete file;
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