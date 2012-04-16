#ifndef TYPES_ARRAYTYPE_H
#define TYPES_ARRAYTYPE_H

#include "basictype.h"

#include "io/netcdf.h"

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
	
	asagi::Grid::Error check(io::NetCdf& file)
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
	
	void load(io::NetCdf &file,
		unsigned long xoffset, unsigned long yoffset, unsigned long zoffset,
		unsigned long xsize, unsigned long ysize, unsigned long zsize,
		void *buf)
	{
		file.getVar<void>(buf, xoffset, yoffset, zoffset, xsize, ysize, zsize);
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
