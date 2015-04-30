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
#include <stdlib.h>
#include <stdio.h>
// #include <sys/unistd.h>

int main (int argc, char** argv)
{
	MPI_Init(&argc, &argv);
	
	asagi_grid* grid = asagi_grid_create(ASAGI_FLOAT);
	
	if (asagi_grid_open(grid, "tests/2dgrid.nc", 0) != ASAGI_SUCCESS) {
		printf("Could not load file\n");
		return 1;
	}
	
	printf("Range X: %f-%f\n", asagi_grid_min(grid, 0), asagi_grid_max(grid, 0));
	printf("Range Y: %f-%f\n", asagi_grid_min(grid, 1), asagi_grid_max(grid, 1));
	
	double pos[] = {5, 10};
	printf("Value at 5x10: %f\n", asagi_grid_get_float(grid, pos, 0));
	
	// Print memory usage
// 	char status_path[100];
// 	snprintf(status_path, 99, "/proc/%d/status", getpid());
// 	
// 	FILE* f = fopen(status_path, "r");
// 	
// 	char buf[256];
// 	while (fgets(buf, sizeof buf, f)) {
// 		printf("%s", buf);
// 	}
// 	fclose(f);
	
	asagi_grid_close(grid);
	
	MPI_Finalize();
	
	return 0;
}
