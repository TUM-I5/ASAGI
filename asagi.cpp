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

#include "grid/simplegridcontainer.h"
#include "grid/adaptivegridcontainer.h"

// Static c++ functions
asagi::Grid* asagi::Grid::create(Type type, unsigned int hint,
	unsigned int levels)
{
	if (hint & ADAPTIVE)
		return new grid::AdaptiveGridContainer(type, false, hint, levels);
	return new grid::SimpleGridContainer(type, false, hint, levels);
}

asagi::Grid* asagi::Grid::createArray(Type basicType, unsigned int hint,
	unsigned int levels)
{
	if (hint & ADAPTIVE)
		return new grid::AdaptiveGridContainer(basicType, true, hint, levels);
	return new grid::SimpleGridContainer(basicType, true, hint, levels);
}

asagi::Grid* asagi::Grid::createStruct(unsigned int count,
	unsigned int blockLength[],
	unsigned long displacements[],
	asagi::Grid::Type types[],
	unsigned int hint,
	unsigned int levels)
{
	if (hint & ADAPTIVE)
		return new grid::AdaptiveGridContainer(count, blockLength,
			displacements, types, hint, levels);
	return new grid::SimpleGridContainer(count, blockLength,
		displacements, types, hint, levels);
}

// C interfae

// Init functions

grid_handle* grid_create(grid_type type, unsigned int hint, unsigned int levels)
{
	return asagi::Grid::create(type, hint, levels);
}

grid_handle* grid_create_array(grid_type basic_type, unsigned int hint, unsigned int levels)
{
	return asagi::Grid::createArray(basic_type, hint, levels);
}

grid_handle* grid_create_struct(unsigned int count,
	unsigned int blockLength[],
	unsigned long displacements[],
	grid_type types[],
	unsigned int hint, unsigned int levels)
{
	return asagi::Grid::createStruct(count, blockLength, displacements,
		types, hint, levels);
}

#ifndef ASAGI_NOMPI
grid_error grid_set_comm(grid_handle* handle, MPI_Comm comm)
{
	return handle->setComm(comm);
}
#endif // ASAIG_NOMPI

grid_error grid_set_param(grid_handle* handle, const char* name,
	const char* value, unsigned int level)
{
	return handle->setParam(name, value, level);
}

grid_error grid_open(grid_handle* handle, const char* filename,
	unsigned int level)
{
	return handle->open(filename, level);
}

// Min/Max functions

double grid_min_x(grid_handle* handle)
{
	return handle->getXMin();
}
double grid_min_y(grid_handle* handle)
{
	return handle->getYMin();
}
double grid_max_x(grid_handle* handle)
{
	return handle->getXMax();
}
double grid_max_y(grid_handle* handle)
{
	return handle->getYMax();
}

double grid_delta_x(grid_handle* handle)
{
	return handle->getXDelta();
}
double grid_delta_y(grid_handle* handle)
{
	return handle->getYDelta();
}
double grid_delta_z(grid_handle* handle)
{
	return handle->getZDelta();
}

unsigned int grid_var_size(grid_handle* handle)
{
	return handle->getVarSize();
}

// 1d functions

unsigned char grid_get_byte_1d(grid_handle* handle, double x, unsigned int level)
{
	return handle->getByte1D(x, level);
}
int grid_get_int_1d(grid_handle* handle, double x, unsigned int level)
{
	return handle->getInt1D(x, level);
}
long grid_get_long_1d(grid_handle* handle, double x, unsigned int level)
{
	return handle->getLong1D(x, level);
}
float grid_get_float_1d(grid_handle* handle, double x, unsigned int level)
{
	return handle->getFloat1D(x, level);
}
double grid_get_double_1d(grid_handle* handle, double x, unsigned int level)
{
	return handle->getDouble1D(x, level);
}
void grid_get_buf_1d(grid_handle* handle, void* buf, double x,
	unsigned int level)
{
	handle->getBuf1D(buf, x, level);
}

// 2d functions

unsigned char grid_get_byte_2d(grid_handle* handle, double x, double y,
	unsigned int level)
{
	return handle->getByte2D(x, y, level);
}
int grid_get_int_2d(grid_handle* handle, double x, double y, unsigned int level)
{
	return handle->getInt2D(x, y, level);
}
long grid_get_long_2d(grid_handle* handle, double x, double y,
	unsigned int level)
{
	return handle->getLong2D(x, y, level);
}
float grid_get_float_2d(grid_handle* handle, double x, double y,
	unsigned int level)
{
	return handle->getFloat2D(x, y, level);
}
double grid_get_double_2d(grid_handle* handle, double x, double y,
	unsigned int level)
{
	return handle->getDouble2D(x, y, level);
}
void grid_get_buf_2d(grid_handle* handle, void* buf, double x, double y,
	unsigned int level)
{
	handle->getBuf2D(buf, x, y, level);
}

// 3d functions

unsigned char grid_get_byte_3d(grid_handle* handle, double x, double y,
	double z, unsigned int level)
{
	return handle->getByte3D(x, y, z, level);
}
int grid_get_int_3d(grid_handle* handle, double x, double y, double z,
	unsigned int level)
{
	return handle->getInt3D(x, y, z, level);
}
long grid_get_long_3d(grid_handle* handle, double x, double y, double z,
	unsigned int level)
{
	return handle->getLong3D(x, y, z, level);
}
float grid_get_float_3d(grid_handle* handle, double x, double y, double z,
	unsigned int level)
{
	return handle->getFloat3D(x, y, z, level);
}
double grid_get_double_3d(grid_handle* handle, double x, double y, double z,
	unsigned int level)
{
	return handle->getDouble3D(x, y, z, level);
}
void grid_get_buf_3d(grid_handle* handle, void* buf, double x, double y, double z,
	unsigned int level)
{
	handle->getBuf3D(buf, x, y, z, level);
}

// destructor

void grid_close(grid_handle* handle)
{
	delete handle;
}
