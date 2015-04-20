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
#include <utility>

#include "utils/logger.h"

#include "grid/constants.h"
#include "io/netcdfreader.h"
#include "mpi/mpicomm.h"
#include "numa/numacomm.h"
#include "perf/counter.h"

namespace types
{
class Type;
}

namespace grid
{

/**
 * Contains implementations for one grid level
 */
namespace level
{

/**
 * @brief Base class for a grid level
 */
template<class Type>
class Level
{
private:
	/** The MPI Communicator used for this level */
	const mpi::MPIComm* m_mpiComm;

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
	unsigned long m_dim[MAX_DIMENSIONS];

	/** Offset of the grid */
	double m_offset[MAX_DIMENSIONS];
	
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
	Level(const Level &other)
		: m_mpiComm(other.m_mpiComm), m_numaComm(other.m_numaComm->copy()),
		  m_numaDomainId(other.m_numaDomainId), m_type(other.m_type),
		  m_typeSize(other.m_typeSize),
		  m_inputFile(other.m_inputFile), m_dims(other.m_dims)
	{
		assert(m_numaDomainId == m_numaComm->domainId());
	}

	Level(const mpi::MPIComm &comm,
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
	 */
	double min(unsigned int n) const
	{
		assert(n < dimensions());

		return m_min[n];
	}

	/**
	 * The maximum range of the grid in dimension <code>n</code>
	 */
	double max(unsigned int n) const
	{
		assert(n < dimensions());

		return m_max[n];
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

		double scaling[MAX_DIMENSIONS];

		// Open NetCDF file
		m_inputFile = new io::NetCdfReader(filename, comm().rank());
		if ((err = m_inputFile->open(varname)) != asagi::Grid::SUCCESS)
			return err;

		m_dims = m_inputFile->dimensions();

		for (unsigned int i = 0; i < m_dims; i++) {
			// Get dimension size
			m_dim[i] = m_inputFile->getSize(i);

			// Get offset and scaling
			m_offset[i] = m_inputFile->getOffset(i);

			scaling[i] = m_inputFile->getScaling(i);
		}

#if 0
		// Set time dimension
		if (m_timeDimension == -1) {
			// Time grid, but time dimension not specified
			m_timeDimension = m_inputFile->getDimensions() - 1;
			logDebug(getMPIRank()) << "Assuming time dimension"
				<< DIMENSION_NAMES[m_timeDimension];
		}

		// Set block size in time dimension
		if ((m_timeDimension >= 0) && (m_blockSize[m_timeDimension] == 0)) {
			logDebug(getMPIRank()) << "Setting block size in time dimension"
				<< DIMENSION_NAMES[m_timeDimension] << "to 1";
			m_blockSize[m_timeDimension] = 1;
		}
#endif

		// Set default block size and calculate number of blocks
		for (unsigned int i = 0; i < m_dims; i++) {
#if 0
			if (m_blockSize[i] == 0)
				// Setting default block size, if not yet set
				m_blockSize[i] = 50;

			// A block size large than the dimension does not make any sense
			if (m_dim[i] < m_blockSize[i]) {
				logDebug(getMPIRank()) << "Shrinking" << DIMENSION_NAMES[i]
					<< "block size to" << m_dim[i];
				m_blockSize[i] = m_dim[i];
			}

			// Integer way of rounding up
			m_blocks[i] = (m_dim[i] + m_blockSize[i] - 1) / m_blockSize[i];
#endif

			m_scalingInv[i] = getInvScaling(scaling[i]);

			// Set min/max
			if (std::isinf(scaling[i])) {
				m_min[i] = -std::numeric_limits<double>::infinity();
				m_max[i] = std::numeric_limits<double>::infinity();
			} else {
				// Warning: min and max are inverted of scaling is negative
				double min = m_offset[i];
				double max = m_offset[i] + scaling[i] * (m_dim[i] - 1);

				if (valuePos == grid::CELL_CENTERED) {
					// Add half a cell on both ends
					min -= scaling[i] * (0.5 - NUMERIC_PRECISION);
					max += scaling[i] * (0.5 - NUMERIC_PRECISION);
				}

				m_min[i] = std::min(min, max);
				m_max[i] = std::max(min, max);
			}
		}

#if 0
		// Set default cache size
		if (m_blocksPerNode < 0)
			// Default value
			m_blocksPerNode = 80;
#endif

		// Init type
		err = m_type->check(*m_inputFile);
		if (err != asagi::Grid::SUCCESS)
			return err;

		m_typeSize = m_type->size();

		return asagi::Grid::SUCCESS;
	}

#if 0
private:
	void getAt(void* buf, types::Type::converter_t converter,
		   double x, double y = 0, double z = 0);
#endif

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
		return m_dim[n];
	}

	/**
	 * Converts from the real world coordinates to the indices
	 * of the file.
	 *
	 * @param pos The real world coordinates
	 * @param index The indices in the file
	 */
	void pos2index(const double* pos, size_t* index)
	{
		for (unsigned int i = 0; i < m_dims; i++) {
			double x = round((pos[i] - m_offset[i]) * m_scalingInv[i]);

			if (x < 0 || x >= m_max[i]) {
				logWarning() << "ASAGI: Coordinate in dimension" << i << " is out of range. Fixing.";
				if (x < 0)
					x = 0;
				else
					x = m_max[i]-1;
			}

			index[i] = x;
		}
	}

#if 0
	/**
	 * @return The number of blocks we should store on this node
	 */
	unsigned long getBlocksPerNode() const
	{
		return m_blocksPerNode;
	}
	
	/**
	 * @return The difference of the 2 hands in the clock algorithm
	 *  configured by the user
	 */
	long getHandsDiff() const
	{
		return m_handSpread;
	}
	
	/**
	 * @return The number of values in each direction in a block
	 */
	const size_t* getBlockSize() const
	{
		return m_blockSize;
	}
	
	/**
	 * @return The number of values in direction i in a block
	 */
	size_t getBlockSize(unsigned int i) const
	{
		return m_blockSize[i];
	}

	/**
	 * @return The number of values in each block
	 */
	unsigned long getTotalBlockSize() const
	{
		return m_blockSize[0] * m_blockSize[1] * m_blockSize[2];
	}
	
	/**
	 * @return The number of blocks in the grid
	 */
	unsigned long getBlockCount() const
	{
		return m_blocks[0] * m_blocks[1] * m_blocks[2];
	}
	
	/**
	 * @return The of blocks that are stored on this node
	 */
	unsigned long getLocalBlockCount()
	{
		//return (getBlockCount() + getMPISize() - 1) / getMPISize();
	}
	
	/**
	 * Calculates the position of <code>block</code> in the grid
	 * 
	 * @param block The global block id
	 * @param[out] pos Position (offset) of the block in each dimension
	 */
	void getBlockPos(unsigned long block,
		size_t *pos) const
	{
		pos[0] = block % m_blocks[0];
		pos[1] = (block / m_blocks[0]) % m_blocks[1];
		pos[2] = block / (m_blocks[0] * m_blocks[1]);
	}
	
	/**
	 * @return The global block id that stores the value at (x, y, z)
	 */
	unsigned long getBlockByCoords(unsigned long x, unsigned long y,
		unsigned long z) const
	{
		return (((z / m_blockSize[2]) * m_blocks[1]
			+ (y / m_blockSize[1])) * m_blocks[0])
			+ (x / m_blockSize[0]);
	}
	
	/**
	 * @param id Global block id
	 * @return The rank, that stores the block
	 */
	int getBlockRank(unsigned long id) const
	{
#ifdef ROUND_ROBIN
		//return id % getMPISize();
#else // ROUND_ROBIN
		return id / getLocalBlockCount();
#endif // ROUND_ROBIN
	}
	
	/**
	 * @param id Global block id
	 * @return The offset of the block on the rank
	 */
	unsigned long getBlockOffset(unsigned long id) const
	{
#ifdef ROUND_ROBIN
		//return id / getMPISize();
#else // ROUND_ROBIN
		return id % getLocalBlockCount();
#endif // ROUND_ROBIN
	}
	
	/**
	 * @param id Local block id
	 * @return The corresponding global id
	 */
	unsigned long getGlobalBlock(unsigned long id) const
	{
#ifdef ROUND_ROBIN
		//return id * getMPISize() + getMPIRank();
#else // ROUND_ROBIN
		return id + getMPIRank() * getLocalBlockCount();
#endif // ROUND_ROBIN
	}
#endif

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
	double getInvScaling(double scaling)
	{
		if ((scaling == 0) || std::isinf(scaling))
			return 0;

		return 1/scaling;
	}

	/**
	 * Implementation for round-to-nearest
	 */
	double round(double value)
	{
		return floor(value + 0.5);
	}
};

}

}

#endif // GRID_LEVEL_LEVEL_H

