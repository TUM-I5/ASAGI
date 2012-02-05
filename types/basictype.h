#ifndef TYPES_BASICTYPE_H
#define TYPES_BASICTYPE_H

#include "io/netcdf.h"

#include "type.h"

namespace types {

template<typename T> class BasicType : public Type
{   
public:
	unsigned int getSize()
	{
		return sizeof(T);
	}
	
	void load(io::NetCdf &file,
		unsigned long xoffset, unsigned long yoffset,
		unsigned long xsize, unsigned long ysize,
		void *buf)
	{
		file.getVar(static_cast<T*>(buf),
			xoffset, yoffset, xsize, ysize);
	}
	
	MPI_Datatype getMPIType();
	
	char getByte(void* buf)
	{
		return *static_cast<T*>(buf);
	}
	
	int getInt(void* buf)
	{
		return *static_cast<T*>(buf);
	}
	
	long getLong(void* buf)
	{
		return *static_cast<T*>(buf);
	}
	
	float getFloat(void* buf)
	{
		return *static_cast<T*>(buf);
	}
	
	double getDouble(void* buf)
	{
		return *static_cast<T*>(buf);
	}
};

template<> MPI_Datatype BasicType<char>::getMPIType();
template<> MPI_Datatype BasicType<int>::getMPIType();
template<> MPI_Datatype BasicType<long>::getMPIType();
template<> MPI_Datatype BasicType<float>::getMPIType();
template<> MPI_Datatype BasicType<double>::getMPIType();

}

#endif // TYPES_BASICTYPE_H
