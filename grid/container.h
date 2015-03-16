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

#ifndef GRID_CONTAINER_H
#define GRID_CONTAINER_H

#include "asagi.h"

#include "constants.h"
#include "mpi/mpicomm.h"
#include "numa/numa.h"
#include "perf/counter.h"
#include "types/type.h"

namespace grid
{

class Grid;

/**
 * A container that stores multiple levels of a grid
 */
class Container
{
private:
	/** The MPI Communicator used for this container */
	const mpi::MPIComm &m_comm;

	/** NUMA "communicator" for this container */
	const numa::Numa &m_numa;

	/**
	 * The type of values we save in the grid.
	 * This class implements all type specific operations.
	 */
	const types::Type * const m_type;

	/** Value Position (cell-centered || vertex-centered) */
	ValuePosition m_valuePos;

protected:
	/** Minimum in x dimension (set by subclasses) */
	double m_minX;
	/** Minimum in y dimension (set by subclasses) */
	double m_minY;
	/** Minimum in z dimension (set by subclasses) */
	double m_minZ;
	/** Maximum in x dimension (set by subclasses) */
	double m_maxX;
	/** Maximum in y dimension (set by subclasses) */
	double m_maxY;
	/** Maximum in z dimension (set by subclasses) */
	double m_maxZ;

public:
	Container(const mpi::MPIComm &comm,
		const numa::Numa &numa,
		const types::Type* type,
		ValuePosition valuePos);
	virtual ~Container();
	
	/**
	 * @brief Initialize a level of the container
	 *
	 * This function is not thread-safe and should only be called by one thread
	 *
	 * @param filename The name of the file for this level
	 * @param varname The variable name in the file
	 * @param level The level that should be initialized
	 * @return
	 */
	virtual asagi::Grid::Error init(const char* filename,
			const char* varname,
			unsigned int level) = 0;
	
	double getMin(unsigned int n) const
	{
		// TODO
		return m_minX;
	}
	double getMax(unsigned int n) const
	{
		// TODO
		return m_maxX;
	}
	
	double getDelta(unsigned int n, unsigned int level) const
	{
		// TODO
		return 0;
	}

	unsigned int getVarSize() const
	{
		return m_type->getSize();
	}

	/**
	 * Write the value at <code>pos</code> into <code>buf</code>.
	 *
	 * @param buf
	 * @param pos
	 * @param converter
	 * @param level
	 */
	virtual void getAt(void* buf, const double* pos,
			types::Type::converter_t converter, unsigned int level = 0) = 0;

	/**
	 * Get a counter for a specific level
	 */
	virtual unsigned long getCounter(perf::Counter::CounterType type,
			unsigned int level = 0) const = 0;

protected:
	/*
	unsigned char getByte1D(double x, unsigned int level = 0)
	{
		return getByte3D(x, 0, 0, level);
	}
	int getInt1D(double x, unsigned int level = 0)
	{
		return getInt3D(x, 0, 0, level);
	}
	long getLong1D(double x, unsigned int level = 0)
	{
		return getLong3D(x, 0, 0, level);
	}
	float getFloat1D(double x, unsigned int level = 0)
	{
		return getFloat3D(x, 0, 0, level);
	}
	double getDouble1D(double x, unsigned int level = 0)
	{
		return getDouble3D(x, 0, 0, level);
	}
	void getBuf1D(void* buf, double x, unsigned int level = 0)
	{
		getBuf3D(buf, x, 0, 0, level);
	}
	
	unsigned char getByte2D(double x, double y, unsigned int level = 0)
	{
		return getByte3D(x, y, 0, level);
	}
	int getInt2D(double x, double y, unsigned int level = 0)
	{
		return getInt3D(x, y, 0, level);
	}
	long getLong2D(double x, double y, unsigned int level = 0)
	{
		return getLong3D(x, y, 0, level);
	}
	float getFloat2D(double x, double y, unsigned int level = 0)
	{
		return getFloat3D(x, y, 0, level);
	}
	double getDouble2D(double x, double y, unsigned int level = 0)
	{
		return getDouble3D(x, y, 0, level);
	}
	void getBuf2D(void* buf, double x, double y, unsigned int level = 0)
	{
		getBuf3D(buf, x, y, 0, level);
	}*/
	
	/**
	 * @return The communicator for this container
	 */
	const mpi::MPIComm& comm() const
	{
		return m_comm;
	}
	
	/**
	 * @return The NUMA communicator for this container
	 */
	const numa::Numa& numa() const
	{
		return m_numa;
	}

	/**
	 * @return The type for this container
	 */
	const types::Type* type() const
	{
		return m_type;
	}

	/**
	 * @return The value position for this container
	 */
	ValuePosition valuePosition() const
	{
		return m_valuePos;
	}

protected:
	grid::Grid* createGrid(unsigned int hint, unsigned int id) const;
};

}

#endif // GRID_CONTAINER_H
