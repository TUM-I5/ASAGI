#ifndef TYPES_TYPE_H
#define TYPES_TYPE_H

#include <mpi.h>

namespace io {
	class NetCdf;
}

namespace types
{

class Type
{
public:
	typedef void (Type::*converter_t)(void*, void*);
	
public:
	virtual bool check(io::NetCdf &file);
	
	/**
	 * @return The size of the variable
	 */
	virtual unsigned int getSize() = 0;
	
	/**
	 * Loads a block form the netcdf file into the buffer
	 */
	virtual void load(io::NetCdf &file,
		unsigned long xoffset, unsigned long yoffset,
		unsigned long xsize, unsigned long ysize,
		void *buf) = 0;
	
	virtual MPI_Datatype getMPIType() = 0;
	
	/**
	 * Conversation functions:
	 * The value is copied from data to buf, doing transformations between
	 * basic types.<br>
	 * E.g.: If the type of this class is "float" and the "convertDouble" is
	 * called, *data should be a float and *buf a double.<br>
	 * All these functions have the type "converter_t". This way it is
	 * possible to pass them arround as pointers.
	 */
	virtual void convertByte(void* data, void* buf) = 0;
	virtual void convertInt(void* data, void* buf) = 0;
	virtual void convertLong(void* data, void* buf) = 0;
	virtual void convertFloat(void* data, void* buf) = 0;
	virtual void convertDouble(void* data, void* buf) = 0;
	void convertBuffer(void* data, void* buf);
};

}

#endif // TYPES_TYPE_H
