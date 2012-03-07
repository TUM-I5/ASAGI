#ifndef TYPES_ARRAYTYPE_H
#define TYPES_ARRAYTYPE_H

#include "type.h"

#include "io/netcdf.h"

namespace types {

template<typename T> class ArrayType : public Type
{
private:
	/** Number of elements in the array */
	unsigned int m_arraySize;
	
	/** The MPI_Datatype representing this type*/
	MPI_Datatype m_mpiType;
	
public:
	ArrayType() : m_mpiType(MPI_DATATYPE_NULL) { }
	
	virtual ~ArrayType()
	{
		MPI_Type_free(&m_mpiType);
	}
	
	asagi::Grid::Error check(io::NetCdf& file)
	{
		if (file.getVarSize() % sizeof(T) != 0)
			return asagi::Grid::INVALID_VAR_SIZE;
		
		m_arraySize = file.getVarSize() / sizeof(T);
		
		// Create the mpi datatype
		if (MPI_Type_contiguous(m_arraySize, getBasicMPIType(),
			&m_mpiType) != MPI_SUCCESS)
			return asagi::Grid::MPI_ERROR;
		if (MPI_Type_commit(&m_mpiType) != MPI_SUCCESS)
			return asagi::Grid::MPI_ERROR;
		
		return asagi::Grid::SUCCESS;
	}
	
	unsigned int getSize()
	{
		return m_arraySize * sizeof(T);
	}
	
	void load(io::NetCdf &file,
		unsigned long xoffset, unsigned long yoffset, unsigned long zoffset,
		unsigned long xsize, unsigned long ysize, unsigned long zsize,
		void *buf)
	{
		file.getVar<void>(buf, xoffset, yoffset, zoffset, xsize, ysize, zsize);
	}
	
	MPI_Datatype getMPIType()
	{
		return m_mpiType;
	}
	
	/**
	 * {@link asagi::Grid::getFloat3D()} & Co. should return the converted
	 * first element of the array
	 */
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
	
private:
	/** The basic MPI type, depends on T */
	MPI_Datatype getBasicMPIType();
};

template<> MPI_Datatype ArrayType<char>::getBasicMPIType();
template<> MPI_Datatype ArrayType<int>::getBasicMPIType();
template<> MPI_Datatype ArrayType<long>::getBasicMPIType();
template<> MPI_Datatype ArrayType<float>::getBasicMPIType();
template<> MPI_Datatype ArrayType<double>::getBasicMPIType();

}

#endif // TYPES_ARRAYTYPE_H
