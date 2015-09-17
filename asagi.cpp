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

#include <cassert>

#include "utils/logger.h"

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

int asagi::Grid::nodeLocalRank(MPI_Comm comm)
{
	// The main idea for this function is taken from:
	// https://blogs.fau.de/wittmann/2013/02/mpi-node-local-rank-determination/
	// http://git.rrze.uni-erlangen.de/gitweb/?p=apsm.git;a=blob;f=MpiNodeRank.cpp;hb=HEAD

	int mpiResult; NDBG_UNUSED(mpiResult);

	typedef char procName_t[MPI_MAX_PROCESSOR_NAME+1];

	// Get the processor name
	procName_t procName;
	int procNameLength;

	MPI_Get_processor_name(procName, &procNameLength);
	assert(procNameLength <= MPI_MAX_PROCESSOR_NAME);

	procName[procNameLength] = '\0';

	// Compute Adler32 hash
	const uint8_t* buffer = reinterpret_cast<const uint8_t*>(procName);
	uint32_t s1 = 1;
	uint32_t s2 = 0;

	for (int i = 0; i < procNameLength; i++) {
		s1 = (s1 + buffer[i]) % 65521;
		s2 = (s2 + s1) % 65521;
	}

	uint32_t hash = (s2 << 16) | s1;
	int rank;
	mpiResult = MPI_Comm_rank(comm, &rank);
	assert(mpiResult == MPI_SUCCESS);

	MPI_Comm nodeComm;
	mpiResult = MPI_Comm_split(comm, hash, rank, &nodeComm);
	assert(mpiResult == MPI_SUCCESS);

	// Gather all proc names of this node to detect Adler32 collisions
	int nodeSize;
	mpiResult = MPI_Comm_size(nodeComm, &nodeSize);
	assert(mpiResult == MPI_SUCCESS);

	procName_t* procNames = new procName_t[nodeSize];

	mpiResult = MPI_Allgather(procName, MPI_MAX_PROCESSOR_NAME+1, MPI_CHAR,
    	procNames, MPI_MAX_PROCESSOR_NAME+1, MPI_CHAR, nodeComm);
	assert(mpiResult == MPI_SUCCESS);

	// recv contains now an array of hostnames from all MPI ranks of
	// this communicator. They are sorted ascending by the MPI rank.
	int nodeRank, realNodeRank = 0;
	mpiResult = MPI_Comm_rank(comm, &nodeRank);
	assert(mpiResult == MPI_SUCCESS);

	for (int i = 0; i < nodeRank; i++) {
		if (strcmp(procName, procNames[i]) == 0)
			// Detect false hash collisions
			realNodeRank++;
	}

	mpiResult = MPI_Comm_free(&nodeComm);
	assert(mpiResult == MPI_SUCCESS);

	delete [] procNames;

	return realNodeRank;
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

int asagi_node_local_rank(MPI_Comm comm)
{
	return asagi::Grid::nodeLocalRank(comm);
}
#endif // ASAGI_NOMPI
