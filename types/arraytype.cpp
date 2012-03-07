#include "arraytype.h"

using namespace types;

template<> MPI_Datatype ArrayType<char>::getBasicMPIType()
{
	return MPI_SIGNED_CHAR;
}

template<> MPI_Datatype ArrayType<int>::getBasicMPIType()
{
	return MPI_INT;
}

template<> MPI_Datatype ArrayType<long>::getBasicMPIType()
{
	return MPI_LONG;
}

template<> MPI_Datatype ArrayType<float>::getBasicMPIType()
{
	return MPI_FLOAT;
}

template<> MPI_Datatype ArrayType<double>::getBasicMPIType()
{
	return MPI_DOUBLE;
}
