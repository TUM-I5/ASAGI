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

#ifndef GRID_GRID_H
#define GRID_GRID_H

#ifdef ECLIPSE_INDEXER
// Workaround for stupid Eclipse indexer
#include <bits/allocator.h>
#endif // ECLIPSE_INDEXER

#include "asagi.h"

#include <map>
#include <string>
#include <vector>

#include "container.h"
#include "fortran/pointerarray.h"
#include "magic/nulltype.h"
#include "mpi/mpicomm.h"
#include "numa/numa.h"
#include "threads/once.h"
#include "types/type.h"

namespace grid
{

/**
 * Basic implementation for an asagi::Grid
 */
class Grid : public asagi::Grid
{
private:
	/**
	 * Creates a container that depends on {@link m_type}
	 */
	template<template<class Level, class Type> class Container,
		template<class Type> class Level, class TypeList>
	class TypeSelector
	{
	public:
		static grid::Container* createContainer(Grid &grid);
	};

	/** Id of the grid, used for the fortran <-> c interface */
	int m_id;

	/**
	 * The type of values we save in the grid.
	 * This class implements all type specific operations.
	 */
	types::Type *m_type;

	/** Dictionary for storing all parameters (one for each level) */
	std::vector<std::map<std::string, std::string>> m_params;

	/** NUMA domain management */
	numa::Numa m_numa;

	/** All grid containers (one for each NUMA domain) */
	std::vector<Container*> m_containers;

	/** Only resize the containers once */
	threads::Once m_resizeOnce;

	/** The communicator */
	mpi::MPIComm m_comm;

public:
	Grid(asagi::Grid::Type type, bool isArray = false);
	Grid(unsigned int count,
		unsigned int blockLength[],
		unsigned long displacements[],
		asagi::Grid::Type types[]);

	virtual ~Grid();

#ifndef ASAGI_NOMPI
	asagi::Grid::Error setComm(MPI_Comm comm = MPI_COMM_WORLD)
	{
		return m_comm.init(comm);
	}
#endif // ASAGI_NOMPI

	asagi::Grid::Error setThreads(unsigned int threads)
	{
		return m_numa.setThreads(threads);
	}

	void setParam(const char* name, const char* value, unsigned int level = 0);

	asagi::Grid::Error open(const char* filename, unsigned int level = 0);

	double getMin(unsigned int n) const
	{
		return m_containers[0]->getMin(n);
	}

	double getMax(unsigned int n) const
	{
		return m_containers[0]->getMax(n);
	}

	double getDelta(unsigned int n, unsigned int level = 0) const
	{
		return m_containers[0]->getDelta(n, level);
	}

	unsigned int getVarSize() const
	{
		return m_type->size();
	}

	unsigned char getByte(const double* pos, unsigned int level = 0)
	{
		unsigned char buf;
		m_containers[m_numa.domainId()]->getByte(&buf, pos, level);

		return buf;
	}

	int getInt(const double* pos, unsigned int level = 0)
	{
		int buf;
		m_containers[m_numa.domainId()]->getInt(&buf, pos, level);

		return buf;
	}

	long getLong(const double* pos, unsigned int level = 0)
	{
		long buf;
		m_containers[m_numa.domainId()]->getLong(&buf, pos, level);

		return buf;
	}

	float getFloat(const double* pos, unsigned int level = 0)
	{
		float buf;
		m_containers[m_numa.domainId()]->getFloat(&buf, pos, level);

		return buf;
	}

	double getDouble(const double* pos, unsigned int level = 0)
	{
		double buf;
		m_containers[m_numa.domainId()]->getDouble(&buf, pos, level);

		return buf;
	}

	void getBuf(void* buf, const double* pos, unsigned int level = 0)
	{
		m_containers[m_numa.domainId()]->getBuf(buf, pos, level);
	}

	unsigned long getCounter(const char* name, unsigned int level = 0);

	/**
	 * Converts the C pointer of the grid to the Fortran identifier
	 *
	 * @return The unique index of the grid container
	 */
	int c2f() const
	{
		return m_id;
	}

private:
	void init();

	void initContainers();

	template<typename T>
	T param(const char* name, T defaultValue, unsigned int level = 0) const;

private:
	/** The index -> pointer translation array */
	static fortran::PointerArray<Grid> m_pointers;

public:
	/**
	 * Converts a Fortran index to a c/c++ pointer
	 */
	static Grid* f2c(int i)
	{
		return m_pointers.get(i);
	}
};

/**
 * @cond ignore
 * Doxygen does not generate correct xhtml for this
 */
template<template<class Level, class Type> class Container,
	template<class Type> class Level>
class Grid::TypeSelector<Container, Level, magic::NullType>
{
public:
	static grid::Container* createContainer(Grid &grid);
};
/**
 * @endcond
 */

}

#endif // GRID_GRID_H
