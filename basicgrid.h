#ifndef BASICGRID_H
#define BASICGRID_H

#include "grid.h"
#include "io/netcdf.h"

template<typename T> class BasicGrid : public Grid
{
private:
	T *values;
	
	T defaultValue;
public:
	BasicGrid()
	{
		values = 0L;
	}
	
	virtual ~BasicGrid()
	{
		delete [] values;
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
		x = round((x - getXOffset()) / getXScaling());
		y = round((y - getYOffset()) / getYScaling());
		return getAt(static_cast<long>(x), static_cast<long>(y));
	}
	
	const T* getAt(long x, long y)
	{
		y = y * getXDim() + x;
		
		// Range check
		if (y < 0)
			return &defaultValue;
		if (static_cast<unsigned long>(y) >= getXDim() * getYDim())
			return &defaultValue;
		
		return &values[y];
	}
	
protected:
	bool open(io::NetCdf &file)
	{
		values = new T[getXDim() * getYDim()];
		
		file.getVar(values);
		
		file.getDefault(defaultValue);
		
		return true;
	}
	
	/**
	 * This function is used by exportPng, which only works on floats
	 */
	float getAtFloat(long x, long y)
	{
		return static_cast<float>(*getAt(x, y));
	}

};

#endif