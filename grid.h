#ifndef GRID_H
#define GRID_H

#include <asagi.h>

#include "fortran/pointerarray.h"

class Grid : public asagi::Grid
{
private:
	int id;
	
protected:
	// variables will be set by child
	unsigned long dimX;
	unsigned long dimY;
	
	float offsetX;
	float offsetY;
	
	float scalingX;
	float scalingY;
public:
	Grid();
	virtual ~Grid();
	float getXMin();
	float getYMin();
	float getXMax();
	float getYMax();
	
	bool exportPng(const char* filename);
	
	// These are not part of the offical interface
	int c2f();
protected:
	virtual float getAtFloat(long x, long y) = 0;
private:
	static fortran::PointerArray<Grid> pointers;
private:
	static void h2rgb(float h, unsigned char &red, unsigned char &green, unsigned char &blue);
protected:
	static float round(float value);
public:
	static Grid* f2c(int i);
};

#endif