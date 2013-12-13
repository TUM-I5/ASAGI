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
 * @copyright 2012 Sebastian Rettenberger <rettenbs@in.tum.de>
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

int main(int argc, char** argv)
{
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
class MPIHelper : public CxxTest::GlobalFixture
{
public:
	bool setUpWorld(void)
	{
		return mainInitSuccess;
	}
	
	bool tearDownWorld(void)
	{
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
