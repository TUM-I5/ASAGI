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
 * @copyright 2012-2015 Sebastian Rettenberger <rettenbs@in.tum.de>
 */

#include "asagi_f90.h"
#include "grid/grid.h"

// Init functions

/**
 * This function is part of the C++ <-> Fortran interface
 */
int f90asagi_grid_create(asagi_type type)
{
	return static_cast<grid::Grid*>(
		asagi::Grid::create(type))->c2f();
}

/**
 * This function is part of the C++ <-> Fortran interface
 */
int f90asagi_grid_create_array(asagi_type basic_type)
{
	return static_cast<grid::Grid*>(
		asagi::Grid::createArray(basic_type))->c2f();
}

/**
 * This function is part of the C++ <-> Fortran interface
 */
int f90asagi_grid_create_struct(int count,
	int blockLength[],
	long displacements[],
	asagi_type types[])
{
	// Not sure of reinterpret_cast is save here
	return static_cast<grid::Grid*>(
		asagi::Grid::createStruct(count,
			reinterpret_cast<unsigned int*>(blockLength),
			reinterpret_cast<unsigned long*>(displacements),
			types))->c2f();
}

/**
 * This function is part of the C++ <-> Fortran interface
 */
void f90asagi_grid_set_comm(int grid_id, int comm)
{
	// This function is a no-op if called on a version without MPI support
#ifndef ASAGI_NOMPI
	// MPI_Comm_f2c expects an MPI_Fint, however iso_c_bindings
	// already converts this parameter into c integer
	grid::Grid::f2c(grid_id)->setComm(MPI_Comm_f2c(comm));
#endif // ASAGI_NOMPI
}

/**
 * This function is part of the C++ <-> Fortran interface
 */
void f90asagi_grid_set_threads(int grid_id, int threads)
{
	grid::Grid::f2c(grid_id)->setThreads(threads);
}

/**
 * This function is part of the C++ <-> Fortran interface
 */
void f90asagi_grid_set_param(int grid_id, const char* name,
	const char* value, int level)
{
	grid::Grid::f2c(grid_id)->setParam(name, value, level);
}

/**
 * This function is part of the C++ <-> Fortran interface
 */
asagi_error f90asagi_grid_open(int grid_id, const char* filename,
	int level)
{
	return grid::Grid::f2c(grid_id)->open(filename, level);
}

// Min/Max functions

/**
 * This function is part of the C++ <-> Fortran interface
 */
double f90asagi_grid_min(int grid_id, int n)
{
	return grid::Grid::f2c(grid_id)->getMin(n);
}
/**
 * This function is part of the C++ <-> Fortran interface
 */
double f90asagi_grid_max(int grid_id, int n)
{
	return grid::Grid::f2c(grid_id)->getMax(n);
}

/**
 * This function is part of the C++ <-> Fortran interface
 */
double f90asagi_grid_delta(int grid_id, int n, int level)
{
	return grid::Grid::f2c(grid_id)->getDelta(n, level);
}

/**
 * This function is part of the C++ <-> Fortran interface
 */
int f90asagi_grid_var_size (int grid_id)
{
	return grid::Grid::f2c(grid_id)->getVarSize();
}

// Getters

/**
 * This function is part of the C++ <-> Fortran interface
 */
unsigned char f90asagi_grid_get_byte(int grid_id, double* pos, int level)
{
	return grid::Grid::f2c(grid_id)->getByte(pos, level);
}
/**
 * This function is part of the C++ <-> Fortran interface
 */
int f90asagi_grid_get_int(int grid_id, double* pos, int level)
{
	return grid::Grid::f2c(grid_id)->getInt(pos, level);
}
/**
 * This function is part of the C++ <-> Fortran interface
 */
long f90asagi_grid_get_long(int grid_id, double* pos, int level)
{
	return grid::Grid::f2c(grid_id)->getLong(pos, level);
}
/**
 * This function is part of the C++ <-> Fortran interface
 */
float f90asagi_grid_get_float(int grid_id, double* pos, int level)
{
	return grid::Grid::f2c(grid_id)->getFloat(pos, level);
}
/**
 * This function is part of the C++ <-> Fortran interface
 */
double f90asagi_grid_get_double(int grid_id, double* pos, int level)
{
	return grid::Grid::f2c(grid_id)->getDouble(pos, level);
}
/**
 * This function is part of the C++ <-> Fortran interface
 */
void f90asagi_grid_get_buf(int grid_id, void* buf, double* pos,
	int level)
{
	grid::Grid::f2c(grid_id)->getBuf(buf, pos, level);
}

// destructor

/**
 * This function is part of the C++ <-> Fortran interface
 */
void f90asagi_grid_close(int grid_id)
{
	delete grid::Grid::f2c(grid_id);
}
