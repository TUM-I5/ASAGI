/**
 * @file
 * 
 * @author Sebastian Rettenberger <rettenbs@in.tum.de>
 * 
 * @brief Contains some useful macros for unit tests
 * 
 * Should be included after the test framework but before other includes
 */

#ifndef TESTS_UNITTEST_GLOBALTEST_H
#define TESTS_UNITTEST_GLOBALTEST_H

#include <mpi.h>
#include <cxxtest/TestSuite.h>
#include <cxxtest/GlobalFixture.h>

/**
 * This currently only works with MPI versions that do not depend on mpiexec
 * (like openmpi).
 */
class MPIHelper : public CxxTest::GlobalFixture
{
public:
	bool setUpWorld(void)
	{
		// TODO run with MPI an get args
		return (MPI_Init(0, 0) == MPI_SUCCESS);
	}
	
	bool tearDownWorld(void)
	{
		return (MPI_Finalize() == MPI_SUCCESS);
	}
};

static MPIHelper mpiHelper;

// We want to access private and protected members in asagi classes
#define private public
#define protected public

#endif // TESTS_UNITTEST_GLOBALTEST_H