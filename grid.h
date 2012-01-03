#ifndef GRID_H
#define GRID_H

#include <asagi.h>

#include "fortran/pointerarray.h"
#include "io/netcdf.h"

class Grid : public asagi::Grid
{
private:
	/** Id of the grid, used for the fortran <-> c interface */
	int id;
	
	unsigned long dimX;
	unsigned long dimY;
	
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
	
	bool exportPng(const char* filename);
	
	// These are not part of the offical interface
	int c2f();
protected:
	unsigned long getXDim();
	unsigned long getYDim();
	
	float getXOffset();
	float getYOffset();
	
	float getXScaling();
	float getYScaling();
	
	virtual bool open(io::NetCdf &file) = 0;
	
	/**
	 * This function is used by exportPng, which only works on floats
	 */
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