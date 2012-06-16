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
 */

#ifndef TYPES_BASICTYPE_H
#define TYPES_BASICTYPE_H

#include "type.h"

#include "io/netcdfreader.h"

namespace types {

/**
 * Implementation for basic types like int, long, float, double
 */
template<typename T> class BasicType : public Type
{   
public:
	virtual unsigned int getSize()
	{
		return sizeof(T);
	}
	
	virtual void load(io::NetCdfReader &file,
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
