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
	/**
	 * Check compatibility of the input file with this type.
	 */
	virtual bool check(io::NetCdf &file)
	{
		// Default: everything is okay
		return true;
	}
	
	/**
	 * @return The size of a single variable
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
	
	virtual char getByte(void* buf) = 0;
	virtual int getInt(void* buf) = 0;
	virtual long getLong(void* buf) = 0;
	virtual float getFloat(void* buf) = 0;
	virtual double getDouble(void* buf) = 0;
};

}

#endif // TYPES_TYPE_H
