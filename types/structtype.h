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

#ifndef TYPES_STRUCTTYPE_H
#define TYPES_STRUCTTYPE_H

#include "basictype.h"

namespace types {

/**
 * Datatype for arbitary structs
 */
template<typename T> class StructType : public BasicType<T>
{
	friend Type* createStruct(unsigned int count, unsigned int blockLength[],
		unsigned long displacements[], asagi::Grid::Type types[]);
private:
	/** Size of this struct */
	unsigned int m_size;
	
#ifndef ASAGI_NOMPI
	/** Number of blocks in this struct */
	unsigned int m_count;
	/** Number of elements in each block */
	int *m_blockLength;
	/** Displacement of each block */
	MPI_Aint *m_displacements;
	/** Datatype of each block */
	MPI_Datatype *m_types;
	
	/** The MPI_Datatype representing this type*/
	MPI_Datatype m_mpiType;
#endif // ASAGI_NOMPI

private:
	StructType(unsigned int count, unsigned int blockLength[],
		unsigned long displacements[], asagi::Grid::Type types[])
	{
#ifndef ASAGI_NOMPI
		m_count = count;
		// We can not create the final mpi datatype, because we do not
		// now, the size yet
		m_blockLength = new int[count+1];
		m_displacements = new MPI_Aint[count+1];
		m_types = new MPI_Datatype[count+1];
		
		for (unsigned int i = 0; i < count; i++) {
			m_blockLength[i] = blockLength[i];
			m_displacements[i] = displacements[i];
			switch (types[i]) {
			case asagi::Grid::BYTE:
				m_types[i] = MPI_BYTE;
				break;
			case asagi::Grid::INT:
				m_types[i] = MPI_INT;
				break;
			case asagi::Grid::LONG:
				m_types[i] = MPI_LONG;
				break;
			case asagi::Grid::FLOAT:
				m_types[i] = MPI_FLOAT;
				break;
			case asagi::Grid::DOUBLE:
				m_types[i] = MPI_DOUBLE;
				break;
			}
		}
#endif // ASAGI_NOMPI
	}
public:
	virtual ~StructType()
	{
#ifndef ASAGI_NOMPI
		MPI_Type_free(&m_mpiType);
		
		delete [] m_blockLength;
		delete [] m_displacements;
		delete [] m_types;
#endif // ASAGI_NOMPI
	}
	
	asagi::Grid::Error check(const io::NetCdfReader &file)
	{
		m_size = file.getVarSize();
		
#ifndef ASAGI_NOMPI
		m_blockLength[m_count] = m_size;
		m_displacements[m_count] = 1;
		m_types[m_count] = MPI_UB;
		
		// Create the mpi datatype
		if (MPI_Type_create_struct(
			m_count+1,
			m_blockLength,
			m_displacements,
			m_types,
			&m_mpiType) != MPI_SUCCESS)
			return asagi::Grid::MPI_ERROR;
		if (MPI_Type_commit(&m_mpiType) != MPI_SUCCESS)
			return asagi::Grid::MPI_ERROR;
		
		// We do not need them anymore
		delete [] m_blockLength;
		delete [] m_displacements;
		delete [] m_types;
		
		m_blockLength = 0L;
		m_displacements = 0L;
		m_types = 0L;
#endif // ASAGI_NOMPI
		
		return asagi::Grid::SUCCESS;
	}
	
	unsigned int getSize()
	{
		return m_size;
	}
	
	void load(io::NetCdfReader &file,
		const size_t *offset,
		const size_t *size,
		void *buf)
	{
		file.getBlock<void>(buf, offset, size, getSize());
	}
	
#ifndef ASAGI_NOMPI
	MPI_Datatype getMPIType()
	{
		return m_mpiType;
	}
#endif // ASAGI_NOMPI
};

Type* createStruct(
	unsigned int count,
	unsigned int blockLength[],
	unsigned long displacements[],
	asagi::Grid::Type types[]);

}

#endif // TYPES_STRUCTTYPE_H
