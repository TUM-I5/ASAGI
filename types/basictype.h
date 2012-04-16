#ifndef TYPES_BASICTYPE_H
#define TYPES_BASICTYPE_H

#include "type.h"

#include "io/netcdf.h"

namespace types {

template<typename T> class BasicType : public Type
{   
public:
	virtual unsigned int getSize()
	{
		return sizeof(T);
	}
	
	virtual void load(io::NetCdf &file,
		unsigned long xoffset, unsigned long yoffset, unsigned long zoffset,
		unsigned long xsize, unsigned long ysize, unsigned long zsize,
		void *buf)
	{
		file.getVar<T>(buf, xoffset, yoffset, zoffset, xsize, ysize, zsize);
	}
	
#ifndef ASAGI_NOMPI
	virtual MPI_Datatype getMPIType();
#endif // ASAGI_NOMPI
	
	void convertByte(void* data, void* buf)
	{
		*static_cast<char*>(buf) = *static_cast<T*>(data);
	}
	
	void convertInt(void* data, void* buf)
	{
		*static_cast<int*>(buf) = *static_cast<T*>(data);
	}
	
	void convertLong(void* data, void* buf)
	{
		*static_cast<long*>(buf) = *static_cast<T*>(data);
	}
	
	void convertFloat(void* data, void* buf)
	{
		*static_cast<float*>(buf) = *static_cast<T*>(data);
	}
	
	void convertDouble(void* data, void* buf)
	{
		*static_cast<double*>(buf) = *static_cast<T*>(data);
	}
};

#ifndef ASAGI_NOMPI
template<> MPI_Datatype BasicType<char>::getMPIType();
template<> MPI_Datatype BasicType<int>::getMPIType();
template<> MPI_Datatype BasicType<long>::getMPIType();
template<> MPI_Datatype BasicType<float>::getMPIType();
template<> MPI_Datatype BasicType<double>::getMPIType();
#endif // ASAGI_NOMPI

}

#endif // TYPES_BASICTYPE_H
