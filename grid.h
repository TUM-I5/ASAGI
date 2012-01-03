#ifndef GRID_H
#define GRID_H

#include <database.h>

#include "fortran/pointerarray.h"

class Grid : public grid::Grid
{
private:
	int id;
	
	float* values;
	unsigned long dimX;
	unsigned long dimY;
	
	float defaultValue;
	
	float offsetX;
	float offsetY;
	
	float scalingX;
	float scalingY;
public:
	Grid();
	virtual ~Grid();
	bool open(const char* filename);
	float getXMin();
	float getYMin();
	float getXMax();
	float getYMax();
	float get(float x, float y);
	
	bool exportPng(const char* filename);
	
	// These are not part of the offical interface
	float getAt(long x, long y);
	int c2f();
private:
	static fortran::PointerArray<Grid> pointers;
private:
	static void h2rgb(float h, unsigned char &red, unsigned char &green, unsigned char &blue);
	static float round(float value);
public:
	static Grid* f2c(int i);
};

#endif