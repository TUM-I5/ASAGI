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

#include <mpi.h>
#include <asagi.h>
#include <stdio.h>

/**
 * C Example
 */
int main(int argc, char** argv)
{
  MPI_Init(&argc, &argv);
  
  asagi_grid* grid = asagi_grid_create(ASAGI_FLOAT);
  asagi_grid_set_comm(grid, MPI_COMM_WORLD);
  // with threads, set number of threads
  asagi_grid_set_threads(grid, 1);
  
  if (asagi_grid_open(grid, "/path/to/netcdf/file.nc", 0) != ASAGI_SUCCESS) {
    printf("Could not load file\n");
    return 1;
  }
  
  double pos[] = {0, 0};
  printf("Value at (0,0): %f\n", asagi_grid_get_float(grid, pos, 0));
  
  asagi_grid_close(grid);
  
  MPI_Finalize();
  
  return 0;
}
