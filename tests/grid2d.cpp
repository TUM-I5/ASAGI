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
 * @copyright 2012-2013 Sebastian Rettenberger <rettenbs@in.tum.de>
 */

#include <asagi.h>
#include <mpi.h>

#define DEBUG_ABORT MPI_Abort(MPI_COMM_WORLD, 1)
#include "debug/dbg.h"

#include "tests.h"

using namespace asagi;

/**
 * Quick ceil integer function
 */
static unsigned int ceil(unsigned int a, unsigned int b)
{
	return (a + b - 1) / b;
}

int main(int argc, char** argv)
{
	int rank;
	
	MPI_Init(&argc, &argv);
	
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	
	Grid* grid = Grid::create(); // FLOAT is default
	
	if (grid->open(NC_2D) != Grid::SUCCESS)
		return 1;
	
	int value;
	
	for (int i = 0; i < NC_WIDTH; i++) {
		for (int j = 0; j < NC_LENGTH; j++) {
			value = j * NC_WIDTH + i;
			if (grid->getInt2D(i, j) != value) {
				dbgError() << "Test failed on rank" << rank << std::endl
					<< "Value at" << i << j << "should be"
					<< value << "but is" << grid->getInt2D(i, j);
				return 1;
			}
		}
	}

	if (grid->getCounter("accesses") != NC_WIDTH * NC_LENGTH) {
		dbgError() << "Counter \"accesses\" should be" << (NC_WIDTH*NC_LENGTH)
				<< "but is" << grid->getCounter("accesses");
	}

	// Assuming 2 MPI processes / 50 values in each dimension per block
	if (grid->getCounter("mpi_transfers") != (ceil(NC_WIDTH, 50)*ceil(NC_LENGTH, 50)/2)) {
		dbgError() << "Counter \"mpi_transfers\" should be" << (ceil(NC_WIDTH, 50)*ceil(NC_LENGTH, 50)/2)
				<< "but is" << grid->getCounter("mpi_transfers");
	}

	if (grid->getCounter("file_loads") != 0) {
		dbgError() << "Counter \"file_loads\" should be 0"
				<< "but is" << grid->getCounter("file_loads");
		return 1;
	}
	
	delete grid;
	
	MPI_Finalize();
	
	return 0;
}
