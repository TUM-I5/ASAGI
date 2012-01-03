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
	unsigned long x;
	unsigned long y;
public:
	Grid();
	virtual ~Grid();
	bool open(const char* filename);
	unsigned long getXDim();
	unsigned long getYDim();
	float get(unsigned long x, unsigned long y);
	
	void exportPNG(const char* filename);
	int c2f();
private:
	static fortran::PointerArray<Grid> pointers;
public:
	static Grid* f2c(int i);
};

#endif