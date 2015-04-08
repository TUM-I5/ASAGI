/**
 * @file
 *  This file is part of ASAGI.
 * 
 *  ASAGI is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as
 *  published by the Free Software Foundation, either version 3 of
 *  the License, or  (at your option) any later version.
 *
 *  ASAGI is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with ASAGI.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Diese Datei ist Teil von ASAGI.
 *
 *  ASAGI ist Freie Software: Sie koennen es unter den Bedingungen
 *  der GNU Lesser General Public License, wie von der Free Software
 *  Foundation, Version 3 der Lizenz oder (nach Ihrer Option) jeder
 *  spaeteren veroeffentlichten Version, weiterverbreiten und/oder
 *  modifizieren.
 *
 *  ASAGI wird in der Hoffnung, dass es nuetzlich sein wird, aber
 *  OHNE JEDE GEWAEHELEISTUNG, bereitgestellt; sogar ohne die implizite
 *  Gewaehrleistung der MARKTFAEHIGKEIT oder EIGNUNG FUER EINEN BESTIMMTEN
 *  ZWECK. Siehe die GNU Lesser General Public License fuer weitere Details.
 *
 *  Sie sollten eine Kopie der GNU Lesser General Public License zusammen
 *  mit diesem Programm erhalten haben. Wenn nicht, siehe
 *  <http://www.gnu.org/licenses/>.
 * 
 * @copyright 2012-2015 Sebastian Rettenberger <rettenbs@in.tum.de>
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
	/**
	 * Check compatibility of the input file with this type.
	 */
	asagi::Grid::Error check(const io::NetCdfReader &file)
	{
		return asagi::Grid::SUCCESS;
	}

	unsigned int size() const
	{
		return sizeof(T);
	}

	/**
	 * Loads a block form the netCDF file into the buffer
	 */
	void load(io::NetCdfReader &file,
		const size_t *offset,
		const size_t *size,
		void *buf) const
	{
		file.getBlock<T>(buf, offset, size);
	}

#ifndef ASAGI_NOMPI
	virtual MPI_Datatype getMPIType() const;
#endif // ASAGI_NOMPI
	
	/**
	 * The value is copied from data to buf, doing transformations between
	 * basic types.
	 */
	template<typename B>
	void convert(const void* data, B* buf) const
	{
		*buf = *static_cast<const T*>(data);
	}

	/**
	 * Uses the "no-conversion" function for void pointers
	 */
	void convert(const void* data, void* buf) const
	{
		Type::copy(data, buf, sizeof(T));
	}
};

#ifndef ASAGI_NOMPI
template<> inline
MPI_Datatype BasicType<unsigned char>::getMPIType() const
{
	return MPI_BYTE;
}

template<> inline
MPI_Datatype BasicType<int>::getMPIType() const
{
	return MPI_INT;
}

template<> inline
MPI_Datatype BasicType<long>::getMPIType() const
{
	return MPI_LONG;
}

template<> inline
MPI_Datatype BasicType<float>::getMPIType() const
{
	return MPI_FLOAT;
}

template<> inline
MPI_Datatype BasicType<double>::getMPIType() const
{
	return MPI_DOUBLE;
}
#endif // ASAGI_NOMPI

}

#endif // TYPES_BASICTYPE_H
