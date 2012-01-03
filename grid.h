#ifndef GRID_H
#define GRID_H

#include "io/netcdf.h"

class Grid
{
private:
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
};

#endif