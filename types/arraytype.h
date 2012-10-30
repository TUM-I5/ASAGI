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

#ifndef TYPES_ARRAYTYPE_H
#define TYPES_ARRAYTYPE_H

#include "basictype.h"

#include "io/netcdfreader.h"

namespace types {

/**
 * We use {@link types::BasicType}. This way {@link #convertByte} and similar
 * functions convert the first element of the array to the correct datatype. One
 * can use {@link #convertBuffer} to get the whole array.
 */
template<typename T> class ArrayType : public BasicType<T>
{
private:
	/** Number of elements in the array */
	unsigned int m_arraySize;
	
#ifndef ASAGI_NOMPI
	/** The MPI_Datatype representing this type*/
	MPI_Datatype m_mpiType;
#endif // ASAGI_NOMPI
	
public:
	ArrayType()
#ifndef ASAGI_NOMPI
		: m_mpiType(MPI_DATATYPE_NULL)
#endif // ASAGI_NOMPI
	{
	}
	
	virtual ~ArrayType()
	{
#ifndef ASAGI_NOMPI
		MPI_Type_free(&m_mpiType);
#endif // ASAGI_NOMPI
	}
	
	asagi::Grid::Error check(const io::NetCdfReader &file)
	{
		if (file.getVarSize() % sizeof(T) != 0)
			return asagi::Grid::INVALID_VAR_SIZE;
		
		m_arraySize = file.getVarSize() / sizeof(T);
		
#ifndef ASAGI_NOMPI
		// Create the mpi datatype
		if (MPI_Type_contiguous(m_arraySize, BasicType<T>::getMPIType(),
			&m_mpiType) != MPI_SUCCESS)
			return asagi::Grid::MPI_ERROR;
		if (MPI_Type_commit(&m_mpiType) != MPI_SUCCESS)
			return asagi::Grid::MPI_ERROR;
#endif // ASAGI_NOMPI
		
		return asagi::Grid::SUCCESS;
	}
	
	unsigned int getSize()
	{
		return m_arraySize * BasicType<T>::getSize();
	}
	
	void load(io::NetCdfReader &file,
		unsigned long xoffset, unsigned long yoffset, unsigned long zoffset,
		unsigned long xsize, unsigned long ysize, unsigned long zsize,
		void *buf)
	{
		// TODO
		size_t offset[] = {xoffset, yoffset, zoffset};
		size_t size[] = {xsize, ysize, zsize};
		file.getBlock<void>(buf, offset, size, getSize());
	}
	
#ifndef ASAGI_NOMPI
	MPI_Datatype getMPIType()
	{
		return m_mpiType;
	}
#endif // ASAGI_NOMPI
};

}

#endif // TYPES_ARRAYTYPE_H
