#ifndef BASICGRID_H
#define BASICGRID_H

#include "grid.h"
#include "io/netcdf.h"

template<typename T> class BasicGrid : public Grid
{
private:
	T *values;
public:
	BasicGrid()
	{
		values = 0L;
	}
	
	virtual ~BasicGrid()
	{
		delete [] values;
	}
	
	unsigned int getVarSize()
	{
		return sizeof(T);
	}
	
	char getByte(float x, float y)
	{
		return static_cast<char>(getAt(x, y));
	}
	
	int getInt(float x, float y)
	{
		return static_cast<int>(getAt(x, y));
	}
	
	long getLong(float x, float y)
	{
		return static_cast<long>(getAt(x, y));
	}
	
	float getFloat(float x, float y)
	{
		return static_cast<float>(getAt(x, y));
	}
	
	double getDouble(float x, float y)
	{
		return static_cast<double>(getAt(x, y));
	}
	
	void getBuf(float x, float y, void* buf)
	{
		*static_cast<T*>(buf) = getAt(x, y);
	}
	
	/**
	 * This function is for testing only
	 * 
	 * @return The i's value of the array
	 */
	T get(long i)
	{
		return values[i];
	}
	
	/**
	 * Public wrapper for {@link getAt(long, long)} for testing
	 */
	const T getAtTest(long x, long y)
	{
		return getAt(x, y);
	}
	
private:
	const T getAt(float x, float y)
	{
		x = round((x - getXOffset()) / getXScaling());
		y = round((y - getYOffset()) / getYScaling());
		return getAt(static_cast<long>(x), static_cast<long>(y));
	}
	
	/**
	 * @return The value of the 2D internal array at (x, y)
	 */
	const T getAt(long x, long y)
	{
		assert(x >= 0 && x < getXDim()
			&& y >= 0 && y < getYDim());
		
		y = y * getXDim() + x;
		
		return values[y];
	}
	
protected:
	bool load(io::NetCdf &file)
	{
		values = new T[getXDim() * getYDim()];
		
		file.getVar(values);
		
		return true;
	}
	
	float getAtFloat(long x, long y)
	{
		return static_cast<float>(getAt(x, y));
	}

};

#endif