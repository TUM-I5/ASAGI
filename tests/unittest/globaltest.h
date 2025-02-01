/**
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 * SPDX-FileCopyrightText: 2012 Sebastian Rettenberger <rettenbs@in.tum.de>
 *
 * @brief Contains some useful macros for unit tests
 *
 * Should be included after the test framework but before other includes
 */

#ifndef TESTS_UNITTEST_GLOBALTEST_H
#define TESTS_UNITTEST_GLOBALTEST_H

#ifndef ASAGI_NOMPI
#include <mpi.h>
#endif
#include <cxxtest/TestSuite.h>
#include <cxxtest/GlobalFixture.h>

static bool mainInitSuccess = true;

int main(int argc, char** argv) {
#ifndef ASAGI_NOMPI
  if (MPI_Init(&argc, &argv) != MPI_SUCCESS)
    mainInitSuccess = false;
#endif

  return CxxTest::ErrorPrinter().run();
}

/**
 * The MPIHelper works together with {@link main} to setup
 * and free MPI resources.
 */
class MPIHelper : public CxxTest::GlobalFixture {
  public:
  bool setUpWorld(void) { return mainInitSuccess; }

  bool tearDownWorld(void) {
#ifndef ASAGI_NOMPI
    return (MPI_Finalize() == MPI_SUCCESS);
#endif

    return true;
  }
};

static MPIHelper mpiHelper;

// We want to access private and protected members in asagi classes
#define private public
#define protected public

#endif // TESTS_UNITTEST_GLOBALTEST_H
