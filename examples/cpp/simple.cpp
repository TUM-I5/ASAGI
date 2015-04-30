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
 * @copyright 2015 Sebastian Rettenberger <rettenbs@in.tum.de>
 */

#include <asagi.h>
#include <mpi.h>
#include <stdio.h>
#include <cassert>

using namespace asagi;

int main (int argc, char **argv)
{
	int rank, size;
	
	MPI_Init(&argc, &argv);
	
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	
	if (rank == 0) {
		MPI_Comm_size(MPI_COMM_WORLD, &size);
		printf("Running on %d nodes\n", size);
	}
	
	Grid* grid = Grid::create(Grid::FLOAT);
	grid->setComm(MPI_COMM_WORLD);
	
	if (grid->open("tests/2dgrid.nc") != Grid::SUCCESS) {
		printf("Could not load file\n");
		return 1;
	}
	
	if (rank == 0) {
		printf("Range X: %f - %f\n", grid->getMin(0), grid->getMax(0));
		printf("Range Y: %f - %f\n", grid->getMin(1), grid->getMax(1));
	}
	
	if (rank == 0) {
		//printf("%f\n", grid->getFloat2D(1100000, 0));
		//printf("%f\n", grid->getFloat2D(-250000, 0));
		double pos0[] = {5, 10};
		printf("Value at 5x10: %f\n", grid->getFloat(pos0));
		printf("Double value at 5x10: %f\n", grid->getDouble(pos0));
		printf("Int value at 5x10: %d\n", grid->getInt(pos0));
	} else {
		double pos1[] = {5, 10.1};
		printf("Value at 5x10.1: %f\n", grid->getFloat(pos1));
		double pos2[] = {-1, -15.32};
		printf("Value at -1x-15.32: %f\n", grid->getFloat(pos2));
	}
	
	delete grid;
	
	MPI_Finalize();
	
	return 0;
}
