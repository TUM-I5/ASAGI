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

#include "asagi.h"

#include "grid/grid.h"
#ifndef ASAGI_NOMPI
#include "mpi/commthread.h"
#endif // ASAGI_NOMPI

// Static c++ functions
asagi::Grid* asagi::Grid::create(Type type)
{
	return new grid::Grid(type);
}

asagi::Grid* asagi::Grid::createArray(Type type)
{
	return new grid::Grid(type, true);
}

asagi::Grid* asagi::Grid::createStruct(unsigned int count,
		unsigned int blockLength[], unsigned long displacements[], Type types[])
{
	return new grid::Grid(count, blockLength, displacements, types);
}

#ifndef ASAGI_NOMPI
asagi::Grid::Error asagi::Grid::startCommThread(int schedCPU, MPI_Comm comm)
{
	return mpi::CommThread::commThread.init(schedCPU, comm);
}

void asagi::Grid::stopCommThread()
{
	mpi::CommThread::commThread.finialize();
}
#endif // ASAGI_NOMPI

// C interfae

// Init functions

asagi_grid* asagi_grid_create(asagi_type type)
{
	return asagi::Grid::create(type);
}

asagi_grid* asagi_grid_create_array(asagi_type basic_type)
{
	return asagi::Grid::createArray(basic_type);
}

asagi_grid* asagi_grid_create_struct(unsigned int count,
	unsigned int blockLength[],
	unsigned long displacements[],
	asagi_type types[])
{
	return asagi::Grid::createStruct(count, blockLength, displacements, types);
}

#ifndef ASAGI_NOMPI
void asagi_grid_set_comm(asagi_grid* handle, MPI_Comm comm)
{
	handle->setComm(comm);
}
#endif // ASAIG_NOMPI

void asagi_grid_set_threads(asagi_grid* handle, unsigned int threads)
{
	handle->setThreads(threads);
}

void asagi_grid_set_param(asagi_grid* handle, const char* name,
	const char* value, unsigned int level)
{
	handle->setParam(name, value, level);
}

asagi_error asagi_grid_open(asagi_grid* handle, const char* filename,
	unsigned int level)
{
	return handle->open(filename, level);
}

// Min/Max functions

double asagi_grid_min(asagi_grid* handle, unsigned int n)
{
	return handle->getMin(n);
}
double asagi_grid_max(asagi_grid* handle, unsigned int n)
{
	return handle->getMax(n);
}

double asagi_grid_delta(asagi_grid* handle, unsigned int n,
		unsigned int level)
{
	return handle->getDelta(n, level);
}

unsigned int asagi_grid_var_size(asagi_grid* handle)
{
	return handle->getVarSize();
}

// Getters

unsigned char asagi_grid_get_byte(asagi_grid* handle, const double* pos,
		unsigned int level)
{
	return handle->getByte(pos, level);
}
int asagi_grid_get_int(asagi_grid* handle, const double* pos,
		unsigned int level)
{
	return handle->getInt(pos, level);
}
long asagi_grid_get_long(asagi_grid* handle, const double* pos,
		unsigned int level)
{
	return handle->getLong(pos, level);
}
float asagi_grid_get_float(asagi_grid* handle, const double* pos,
		unsigned int level)
{
	return handle->getFloat(pos, level);
}
double asagi_grid_get_double(asagi_grid* handle, const double* pos,
		unsigned int level)
{
	return handle->getDouble(pos, level);
}
void asagi_grid_get_buf(asagi_grid* handle, void* buf, const double* pos,
	unsigned int level)
{
	handle->getBuf(buf, pos, level);
}

// destructor

void asagi_grid_close(asagi_grid* handle)
{
	delete handle;
}

#ifndef ASAGI_NOMPI
asagi_error asagi_start_comm_thread(int sched_cpu, MPI_Comm comm)
{
	return asagi::Grid::startCommThread(sched_cpu, comm);
}

void asagi_stop_comm_thread()
{
	asagi::Grid::stopCommThread();
}
#endif // ASAGI_NOMPI
