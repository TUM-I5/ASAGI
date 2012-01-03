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
	 * This function is for testing only
	 * <br>
	 * Returns the value without range check
	 */
	const T getNoDefault(float x, float y)
	{
		x = round((x - getXOffset()) / getXScaling());
		y = round((y - getYOffset()) / getYScaling());
		
		return getNoDefault(static_cast<long>(x), static_cast<long>(y));
	}
	
	/**
	 * This function is for testing only
	 * <br>
	 * Returns the value without range check
	 */
	const T getNoDefault(long x, long y)
	{
		return values[y * getXDim() + x];
	}
	
private:
	const T getAt(float x, float y)
	{
		x = round((x - getXOffset()) / getXScaling());
		y = round((y - getYOffset()) / getYScaling());
		return getAt(static_cast<long>(x), static_cast<long>(y));
	}
	
	const T getAt(long x, long y)
	{
		y = y * getXDim() + x;
		
		// Range check
		if (y < 0 ||
			static_cast<unsigned long>(y) >= getXDim() * getYDim())
			return defaultValue;
		
		return values[y];
	}
	
protected:
	bool load(io::NetCdf &file)
	{
		values = new T[getXDim() * getYDim()];
		
		file.getVar(values);
		
		file.getDefault(&defaultValue);
		
		return true;
	}
	
	float getAtFloat(long x, long y)
	{
		return static_cast<float>(getAt(x, y));
	}

};

#endif