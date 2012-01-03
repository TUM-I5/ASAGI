#ifndef GRID_H
#define GRID_H

#include <database.h>

#include "fortran/pointerarray.h"
#include "io/netcdf.h"

class Grid : public grid::Grid
{
private:
	int id;
	
	io::NetCdf* file;
	float* values;
	unsigned long dimX;
	unsigned long dimY;
public:
	Grid();
	virtual ~Grid();
	bool open(const char* filename);
	unsigned long getXDim();
	unsigned long getYDim();
	float get(unsigned long x, unsigned long y);
	
	bool exportPng(const char* filename);
	int c2f();
private:
	static fortran::PointerArray<Grid> pointers;
private:
	static void h2rgb(float h, unsigned char &red, unsigned char &green, unsigned char &blue);
public:
	static Grid* f2c(int i);
};

#endif