#include "basictype.h"

using namespace types;

#ifndef ASAGI_NOMPI
template<> MPI_Datatype BasicType<char>::getMPIType()
{
	return MPI_BYTE;
}

template<> MPI_Datatype BasicType<int>::getMPIType()
{
	return MPI_INT;
}

template<> MPI_Datatype BasicType<long>::getMPIType()
{
	return MPI_LONG;
}

template<> MPI_Datatype BasicType<float>::getMPIType()
{
	return MPI_FLOAT;
}

template<> MPI_Datatype BasicType<double>::getMPIType()
{
	return MPI_DOUBLE;
}
#endif // ASAGI_NOMPI