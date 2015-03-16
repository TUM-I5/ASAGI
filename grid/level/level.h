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

#include "utils/logger.h"

#include "grid/constants.h"
#include "mpi/mpicomm.h"
#include "numa/numa.h"
#include "perf/counter.h"

namespace io
{
class NetCdfReader;
}

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
class Level
{
private:
	/** The MPI Communicator used for this level */
	const mpi::MPIComm* m_comm;

	/** NUMA "communicator" for this level */
	const numa::Numa* m_numa;

	/**
	 * The type of values we save in the grid.
	 * This class implements all type specific operations.
	 */
	const types::Type* m_type;

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

#if 0
	/** Number of blocks in x, y and z dimension */
	unsigned long m_blocks[MAX_DIMENSIONS];
	
	/** Number of values in x, y and z dimension in one block */
	size_t m_blockSize[MAX_DIMENSIONS];
	
	/** Number of cached blocks on each node */
	long m_blocksPerNode;
	
	/**
	 * Difference between the hands of the 2-handed clock algorithm.
	 * Subclasses my require this to initialize the
	 * {@link blocks::BlockManager}.
	 */
	long m_handSpread;
	
	/**
	 * 0, 1 or 2 if x, y or z is a time dimension (z is default if
	 * the HAS_TIME hint is specified);
	 * -2 if we don't have any time dimension;
	 * -1 if the time dimension is not (yet) specified
	 * <br>
	 * Declare as signed, to remove compiler warning
	 */
	signed char m_timeDimension;
#endif

	/** Access counters for this grid (level) */
	perf::Counter m_counter;

public:
	Level();

	virtual ~Level();

	/**
	 * Get the current counter for a specific type
	 */
	unsigned long getCounter(perf::Counter::CounterType type) const
	{
		return m_counter.get(type);
	}
	
protected:
	asagi::Grid::Error _init(
			const mpi::MPIComm &comm,
			const numa::Numa &numa,
			const types::Type* type,
			const char* filename,
			const char* varname,
			grid::ValuePosition valuePos);

#if 0
private:
	void getAt(void* buf, types::Type::converter_t converter,
		   double x, double y = 0, double z = 0);
#endif
	
	/**
	 * @return The input file used for this grid
	 */
	io::NetCdfReader& inputFile() const
	{
		return *m_inputFile;
	}
	
	/**
	 * @return The type for this grid
	 */
	const types::Type* type() const
	{
		return m_type;
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
	 * @return The number of cells in x dimension
	 */
	unsigned long getXDim() const
	{
		return m_dim[0];
	}
	/**
	 * @return The number of cells in y dimension
	 */
	unsigned long getYDim() const
	{
		return m_dim[1];
	}
	/**
	 * @return The number of cells in z dimension
	 */
	unsigned long getZDim() const
	{
		return m_dim[2];
	}
	
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
	
	/**
	 * This function is called after opening the NetCDF file. Subclasses
	 * should override it to initialize the grid.
	 */
	virtual asagi::Grid::Error init() = 0;
	
	/**
	 * Subclasses should override this and return false, if they still need
	 * to access the input file after initialization.
	 * 
	 * @return True if the input file should be accessable after
	 * {@link init()} was called.
	 */
	virtual bool keepFileOpen() const
	{
		return false;
	}
	
	/**
	 * Writes the value at the specified index into the buffer, after
	 * converting it with the converter
	 */
	virtual void getAt(void* buf, types::Type::converter_t converter,
		unsigned long x, unsigned long y = 0, unsigned long z = 0) = 0;
#endif

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

	static double getInvScaling(double scaling);

	static double round(double value);
};

}

}

#endif // GRID_LEVEL_LEVEL_H

