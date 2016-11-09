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

#ifndef GRID_LEVEL_LEVEL_H
#define GRID_LEVEL_LEVEL_H

#include "asagi.h"

#include <cassert>
#include <cmath>
#include <utility>

#include "utils/logger.h"

#include "grid/constants.h"
#include "io/netcdfreader.h"
#include "mpi/mpicomm.h"
#include "numa/numacomm.h"
#include "perf/counter.h"

namespace grid
{

/**
 * Contains implementations for one grid level
 */
namespace level
{

/**
 * @brief Base class for a grid level
 *
 * @warning This class also helps mapping from the Fortran order of the
 *  internal representation to the C order of the public interface.
 */
template<class Type>
class Level
{
private:
	/** The MPI Communicator used for this level */
	mpi::MPIComm* m_mpiComm;

	/** NUMA communicator for this level */
	numa::NumaComm* m_numaComm;

	/** The NUMA domain identifier for this instance */
	unsigned int m_numaDomainId;

	/**
	 * The type of values we save in the grid.
	 * This class implements all type specific operations.
	 */
	Type* m_type;

	/** The size of the type */
	unsigned int m_typeSize;

	/** The file that contains this grid */
	io::NetCdfReader *m_inputFile;
	
	/** Total number of dimensions */
	unsigned int m_dims;

	/** Total number of elements in each dimension */
	unsigned long m_size[MAX_DIMENSIONS];

	/** Offset of the grid */
	double m_offset[MAX_DIMENSIONS];
	
	/** The difference between to grid points */
	double m_scaling[MAX_DIMENSIONS];

	/** Minimum possible coordinate in each dimension */
	double m_min[MAX_DIMENSIONS];
	/** Maximum possible coordinate in each dimension */
	double m_max[MAX_DIMENSIONS];

	/**
	 * 1/scaling in most cases (exceptions: scaling = 0
	 * and scaling = inf), used to convert coordinates to indices
	 */
	double m_scalingInv[MAX_DIMENSIONS];

	/** Access counters for this grid (level) */
	perf::Counter m_counter;

public:
	/**
	 * Specialized copy constructor
	 */
	Level(const Level &other)
		: m_mpiComm(other.m_mpiComm), m_numaComm(other.m_numaComm->copy()),
		  m_numaDomainId(other.m_numaDomainId), m_type(other.m_type),
		  m_typeSize(other.m_typeSize),
		  m_inputFile(other.m_inputFile), m_dims(other.m_dims)
	{
		assert(m_numaDomainId == m_numaComm->domainId());
	}

	/**
	 * Constructs a new grid level
	 */
	Level(mpi::MPIComm &comm,
			const numa::Numa &numa,
			Type &type)
		: m_mpiComm(&comm), m_numaComm(numa.createComm()),
		  m_numaDomainId(numa.domainId()), m_type(&type),
		  m_typeSize(0L),
		  m_inputFile(0L), m_dims(0)
	{
	}

	virtual ~Level()
	{
		delete m_inputFile;
		if (m_numaDomainId == 0)
			delete m_numaComm;
	}

	/**
	 * Get the current counter for a specific type
	 */
	unsigned long getCounter(perf::Counter::CounterType type) const
	{
		return m_counter.get(type);
	}
	
	/**
	 * @return The number of dimensions
	 */
	unsigned int dimensions() const
	{
		return m_dims;
	}

	/**
	 * The minimum range of the grid in dimension <code>n</code>
	 *
	 * @warning Dimensions are in C order
	 */
	double min(unsigned int n) const
	{
		assert(n < dimensions());

		return m_min[dimensions()-n-1];
	}

	/**
	 * The maximum range of the grid in dimension <code>n</code>
	 *
	 * @warning Dimensions are in C order
	 */
	double max(unsigned int n) const
	{
		assert(n < dimensions());

		return m_max[dimensions()-n-1];
	}

	/**
	 * The difference between to variables in dimension <code>n</code>
	 *
	 * @warning Dimensions are in C order
	 */
	double delta(unsigned int n) const
	{
		assert(n < dimensions());

		return m_scaling[dimensions()-n-1];
	}

protected:
	/**
	 * Initialize the grid level
	 */
	asagi::Grid::Error open(
			const char* filename,
			const char* varname,
			grid::ValuePosition valuePos)
	{
		// Check for NUMA errors from the constructor
		if (m_numaComm == 0L)
			return asagi::Grid::THREAD_ERROR;

		asagi::Grid::Error err;

		// Open NetCDF file
		m_inputFile = new io::NetCdfReader(filename, comm().rank());
		if ((err = m_inputFile->open(varname)) != asagi::Grid::SUCCESS)
			return err;

		m_dims = m_inputFile->dimensions();

		for (unsigned int i = 0; i < m_dims; i++) {
			// Get dimension size
			m_size[i] = m_inputFile->getSize(i);

			// Get offset and scaling
			m_offset[i] = m_inputFile->getOffset(i);

			m_scaling[i] = m_inputFile->getScaling(i);
		}

		// Set default block size and calculate number of blocks
		for (unsigned int i = 0; i < m_dims; i++) {
			m_scalingInv[i] = getInvScaling(m_scaling[i]);

			// Set min/max
			if (std::isinf(m_scaling[i])) {
				m_min[i] = -std::numeric_limits<double>::infinity();
				m_max[i] = std::numeric_limits<double>::infinity();
			} else {
				// Warning: min and max are inverted of scaling is negative
				double min = m_offset[i];
				double max = m_offset[i] + m_scaling[i] * (m_size[i] - 1);

				if (valuePos == grid::CELL_CENTERED) {
					// Add half a cell on both ends
					min -= m_scaling[i] * (0.5 - NUMERIC_PRECISION);
					max += m_scaling[i] * (0.5 - NUMERIC_PRECISION);
				}

				m_min[i] = std::min(min, max);
				m_max[i] = std::max(min, max);
				m_scaling[i] = std::abs(m_scaling[i]);
			}
		}

		// Init type
		err = m_type->check(*m_inputFile);
		if (err != asagi::Grid::SUCCESS)
			return err;

		m_typeSize = m_type->size();

		return asagi::Grid::SUCCESS;
	}

	/**
	 * @return The MPI communicator
	 */
	mpi::MPIComm& comm()
	{
		return *m_mpiComm;
	}

	/**
	 * @return The MPI communicator
	 */
	const mpi::MPIComm& comm() const
	{
		return *m_mpiComm;
	}

	/**
	 * @return The NUMA communicator
	 */
	numa::NumaComm& numa()
	{
		return *m_numaComm;
	}

	/**
	 * @copydoc numa()
	 */
	const numa::NumaComm& numa() const
	{
		return *m_numaComm;
	}

	/**
	 * @return The domain ID on which this level was created.
	 *  Should be the same as <code>numa().domainId()</code>
	 *  except for the destructor.
	 */
	unsigned int numaDomainId() const
	{
		return m_numaDomainId;
	}

	/**
	 * @return The type for this grid
	 */
	const Type& type() const
	{
		return *m_type;
	}
	
	/**
	 * Does the same as <code>type().size()</code> but faster
	 *
	 * @return The size of the type
	 */
	unsigned int typeSize() const
	{
		return m_typeSize;
	}

	/**
	 * @return The input file used for this grid
	 */
	io::NetCdfReader& inputFile() const
	{
		return *m_inputFile;
	}

	/**
	 * @return The number of cells in dimension n
	 */
	unsigned long size(unsigned int n) const
	{
		return m_size[n];
	}

	/**
	 * Converts from the real world coordinates to the indices
	 * of the file.
	 *
	 * @param pos The real world coordinates (in C order)
	 * @param index The indices in the file (in Fortran order)
	 */
	void pos2index(const double* pos, size_t* index)
	{
		for (int i = m_dims-1; i >= 0; i--) {
			double x = round((pos[m_dims-i-1] - m_offset[i]) * m_scalingInv[i]);

			if (x < 0 || x >= m_size[i]) {
				logWarning() << "ASAGI: Coordinate in dimension" << (m_dims-i-1) << " is out of range. Fixing.";
				if (x < 0)
					x = 0;
				else
					x = m_size[i]-1;
			}

			index[i] = x;
		}
	}

	/**
	 * Close the input file immediately
	 */
	void closeInputFile()
	{
		delete m_inputFile;
		m_inputFile = 0L;
	}

	/**
	 * Used by subclasses to increment counter
	 */
	void incCounter(perf::Counter::CounterType type)
	{
		m_counter.inc(type);
	}

private:
	/** The smallest number we can represent in a double */
	static constexpr double NUMERIC_PRECISION = 1e-10;

	/**
	 * Calculates 1/scaling, except for scaling = 0 and scaling = inf. In this
	 * case it returns 0
	 */
	static double getInvScaling(double scaling)
	{
		if ((scaling == 0) || std::isinf(scaling))
			return 0;

		return 1/scaling;
	}

	/**
	 * Implementation for round-to-nearest
	 */
	static double round(double value)
	{
		return std::floor(value + 0.5);
	}
};

}

}

#endif // GRID_LEVEL_LEVEL_H

