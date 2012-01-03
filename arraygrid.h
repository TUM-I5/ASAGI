#ifndef ARRAYGRID_H
#define ARRAYGRID_H

#include "grid.h"
#include "io/netcdf.h"

template <typename T> class ArrayGrid : public Grid
{
private:
	T *values;
	
	T *defaultValue;
public:
	ArrayGrid()
	{
		values = 0L;
		defaultValue = 0L;
	}
	
	virtual ~ArrayGrid()
	{
		delete [] values;
		delete [] defaultValue;
	}
	
protected:
	bool open(io::NetCdf &file)
	{
		// TODO
		return false;
	}
};

#endif // ARRAYGRID_H
