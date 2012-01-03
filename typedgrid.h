#ifndef TYPEDGRID_H
#define TYPEDGRID_H

#include "grid.h"
#include "io/netcdf.h"

template<typename T> class TypedGrid : public Grid
{
private:
	T *values;
	
	T defaultValue;
public:
	TypedGrid()
	{
		values = 0L;
	}
	
	virtual ~TypedGrid()
	{
		delete [] values;
	}
	
	bool open(const char* filename)
	{
		io::NetCdf file(filename);
		if (!file.open())
			return false;
		
		dimX = file.getXDim();
		dimY = file.getYDim();
		
		values = new T[dimX * dimY];
		
		file.getVar(values);
		
		file.getDefault(defaultValue);
		
		offsetX = file.getXOffset();
		offsetY = file.getYOffset();
		
		scalingX = file.getXScaling();
		scalingY = file.getYScaling();
		
		return true;
	}
	
	int getInt(float x, float y)
	{
		return static_cast<int>(*getAt(x, y));
	}
	
	long getLong(float x, float y)
	{
		return static_cast<long>(*getAt(x, y));
	}
	
	float getFloat(float x, float y)
	{
		return static_cast<float>(*getAt(x, y));
	}
	
	double getDouble(float x, float y)
	{
		return static_cast<double>(*getAt(x, y));
	}
	
private:
	const T* getAt(float x, float y)
	{
		x = round((x - offsetX) / scalingX);
		y = round((y - offsetY) / scalingY);
		return getAt(static_cast<long>(x), static_cast<long>(y));
	}
	
	const T* getAt(long x, long y)
	{
		y = y * dimX + x;
		
		// Range check
		if (y < 0)
			return &defaultValue;
		if (static_cast<unsigned long>(y) >= dimX * dimY)
			return &defaultValue;
		
		return &values[y];
	}
	
protected:
	/**
	 * This function is used by exportPng, which only works on floats
	 */
	float getAtFloat(long x, long y)
	{
		return static_cast<float>(*getAt(x, y));
	}

};

#endif