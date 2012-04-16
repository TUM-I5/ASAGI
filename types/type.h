#ifndef TYPES_TYPE_H
#define TYPES_TYPE_H

#include <asagi.h>

#include <cstring>

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
	/**
	 * Empty destructor, makes sure constructor
	 * of child classes is called
	 */
	virtual ~Type() { }
	
	/**
	 * Check compatibility of the input file with this type.
	 */
	virtual asagi::Grid::Error check(io::NetCdf &file)
	{
		// Default: everything is okay
		return asagi::Grid::SUCCESS;
	}
	
	/**
	 * @return The size of the variable
	 */
	virtual unsigned int getSize() = 0;
	
	/**
	 * Loads a block form the netcdf file into the buffer
	 */
	virtual void load(io::NetCdf &file,
		unsigned long xoffset, unsigned long yoffset, unsigned long zoffset,
		unsigned long xsize, unsigned long ysize, unsigned long zsize,
		void *buf) = 0;
	
#ifndef ASAGI_NOMPI
	virtual MPI_Datatype getMPIType() = 0;
#endif // ASAGI_NOMPI
	
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
	/**
	 * This is the "no-conversation" function. It simple copys a whole
	 * variable from data to buf
	 */
	void convertBuffer(void* data, void* buf)
	{
		memcpy(buf, data, getSize());
	}
};

}

#endif // TYPES_TYPE_H
