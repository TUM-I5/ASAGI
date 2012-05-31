/**
 * @file
 *  This file is part of ASAGI.
 * 
 *  ASAGI is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  ASAGI is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with ASAGI.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Diese Datei ist Teil von ASAGI.
 *
 *  ASAGI ist Freie Software: Sie koennen es unter den Bedingungen
 *  der GNU General Public License, wie von der Free Software Foundation,
 *  Version 3 der Lizenz oder (nach Ihrer Option) jeder spaeteren
 *  veroeffentlichten Version, weiterverbreiten und/oder modifizieren.
 *
 *  ASAGI wird in der Hoffnung, dass es nuetzlich sein wird, aber
 *  OHNE JEDE GEWAEHELEISTUNG, bereitgestellt; sogar ohne die implizite
 *  Gewaehrleistung der MARKTFAEHIGKEIT oder EIGNUNG FUER EINEN BESTIMMTEN
 *  ZWECK. Siehe die GNU General Public License fuer weitere Details.
 *
 *  Sie sollten eine Kopie der GNU General Public License zusammen mit diesem
 *  Programm erhalten haben. Wenn nicht, siehe <http://www.gnu.org/licenses/>.
 * 
 * @copyright 2012 Sebastian Rettenberger <rettenbs@in.tum.de>
 * @version \$Id$
 */

#ifndef TYPES_TYPE_H
#define TYPES_TYPE_H

#include <asagi.h>

#include <cstring>

namespace io {
	class NetCdfReader;
}

/**
 * @brief Types specific code
 */
namespace types
{

/**
 * Describes the type of the variable stored in each grid cell.
 * 
 * This is a base class for arbitrary types with some default implemtentations.
 */
class Type
{
public:
	/**
	 * Describes a function that converts form one basic type to another.
	 * @see convertByte
	 * @see convertInt
	 * @see convertLong
	 * @see convertFloat
	 * @see convertDouble
	 * @see convertBuffer
	 */
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
	virtual asagi::Grid::Error check(const io::NetCdfReader &file)
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
	virtual void load(io::NetCdfReader &file,
		unsigned long xoffset, unsigned long yoffset, unsigned long zoffset,
		unsigned long xsize, unsigned long ysize, unsigned long zsize,
		void *buf) = 0;
	
#ifndef ASAGI_NOMPI
	/**
	 * @return The corresponding MPI_Datatype for this type
	 */
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
	/**
	 * @see convertByte
	 */
	virtual void convertInt(void* data, void* buf) = 0;
	/**
	 * @see convertByte
	 */
	virtual void convertLong(void* data, void* buf) = 0;
	/**
	 * @see convertByte
	 */
	virtual void convertFloat(void* data, void* buf) = 0;
	/**
	 * @see convertByte
	 */
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
