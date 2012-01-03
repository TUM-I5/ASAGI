#include <math.h>

#include "grid.h"
#include "io/png.h"

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
	
	dimX = file->getXDim();
	dimY = file->getYDim();
	values = file->getAll();
	
	return true;
}

unsigned long Grid::getXDim()
{
	return dimX;
}

unsigned long Grid::getYDim()
{
	return dimY;
}

float Grid::get(unsigned long x, unsigned long y)
{
	// no range checking for x and y for performance reason
	
	if (file->isDimSwitched()) {
		// switch x and y
		x ^= y;
		y ^= x;
		x ^= y;
		
		x = x * getXDim() + y;
	} else
		x = x * getYDim() + y;
	
	return values[x];
}

bool Grid::exportPng(const char* filename)
{
	float min, max, value;
	unsigned char red, green, blue;
	
	min = max = get(0, 0);
	for (unsigned long i = 0; i < getXDim(); i++) {
		for (unsigned long j = 0; j < getYDim(); j++) {
			value = get(i, j);
			if (value < min)
				min = value;
			if (value > max)
				max = value;
		}
	}
	
	io::Png png(getXDim(), getYDim());
	if (!png.create(filename))
		return false;
	
	for (unsigned long i = 0; i < getXDim(); i++) {
		for (unsigned long j = 0; j < getYDim(); j++) {
			// do some magic here
			h2rgb((get(i, j) - min) / (max - min) * 2 / 3, red, green, blue);
			png.write(i, getYDim() - j - 1, red, green, blue);
		}
	}
	
	png.close();
	
	return true;
}

int Grid::c2f()
{
	return id;
}

// Fortran <-> c translation array
fortran::PointerArray<Grid> Grid::pointers;

void Grid::h2rgb(float h, unsigned char &red, unsigned char &green, unsigned char &blue)
{
	// h from 0..1
	
	h *= 6;
	float x = fmod(h, 2) - 1;
	if (x < 0)
		x *= -1;
	x = 1 - x;
	
	// <= checks are not 100% correct, it should be <
	// but it solves the "larges-value" issue
	if (h <= 1) {
		red = 255;
		green = x * 255;
		blue = 0;
		return;
	}
	if (h <= 2) {
		red = x * 255;
		green = 255;
		blue = 0;
		return;
	}
	if (h <= 3) {
		red = 0;
		green = 255;
		blue = x * 255;
		return;
	}
	if (h <= 4) {
		red = 0;
		green = x * 255;
		blue = 255;
		return;
	}
	if (h <= 5) {
		red = x * 255;
		green = 0;
		blue = 255;
	}
	// h < 6
	red = 255;
	green = 0;
	blue = x * 255;
}

Grid* Grid::f2c(int i)
{
	return pointers.get(i);
}