#include <math.h>

#ifdef WITH_PNG
#include "io/png.h"
#endif

#include "grid.h"

Grid::Grid()
{
	// Prepare for fortran <-> c translation
	id = pointers.add(this);
}

Grid::~Grid()
{
	// Remove from fortran <-> c translation
	pointers.remove(id);
}

bool Grid::open(const char* filename)
{
	io::NetCdf file(filename);
	if (!file.open())
		return false;
	
	dimX = file.getXDim();
	dimY = file.getYDim();
	
	offsetX = file.getXOffset();
	offsetY = file.getYOffset();
	
	scalingX = file.getXScaling();
	scalingY = file.getYScaling();
	
	return open(file);
}

float Grid::getXMin()
{
	return offsetX + std::min(0.f, dimX * scalingX);
}

float Grid::getYMin()
{
	return offsetY + std::min(0.f, dimY * scalingY);
}

float Grid::getXMax()
{
	return offsetX + std::max(0.f, dimX * scalingX);
}

float Grid::getYMax()
{
	return offsetY + std::max(0.f, dimY * scalingY);
}

bool Grid::exportPng(const char* filename)
{
#ifdef WITH_PNG
	float min, max, value;
	unsigned char red, green, blue;
	
	min = max = getAtFloat(0, 0);
	for (unsigned long i = 0; i < dimX; i++) {
		for (unsigned long j = 0; j < dimY; j++) {
			value = getAtFloat(i, j);
			if (value < min)
				min = value;
			if (value > max)
				max = value;
		}
	}
	
	io::Png png(dimX, dimY);
	if (!png.create(filename))
		return false;
	
	for (unsigned long i = 0; i < dimX; i++) {
		for (unsigned long j = 0; j < dimY; j++) {
			// do some magic here
			h2rgb((getAtFloat(i, j) - min) / (max - min) * 2 / 3, red, green, blue);
			png.write(i, dimY - j - 1, red, green, blue);
		}
	}
	
	png.close();
	
	return true;
#else // WITH_PNG
	// TODO generate a warning or something like this
	return false;
#endif // WITH_PNG
}

/**
 * Converts the C pointer of the grid to the Fortran identifier
 * 
 * @return The unique index of the grid
 */
int Grid::c2f()
{
	return id;
}

long unsigned Grid::getXDim()
{
	return dimX;
}

long unsigned Grid::getYDim()
{
	return dimY;
}

float Grid::getXOffset()
{
	return offsetX;
}

float Grid::getYOffset()
{
	return offsetY;
}

float Grid::getXScaling()
{
	return scalingX;
}

float Grid::getYScaling()
{
	return scalingY;
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

float Grid::round(float value)
{
	return floor(value + 0.5);
}

Grid* Grid::f2c(int i)
{
	return pointers.get(i);
}