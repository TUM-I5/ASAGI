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

#ifndef GRID_LEVEL_BLOCKED_H
#define GRID_LEVEL_BLOCKED_H

#include "level.h"

namespace grid
{

namespace level
{

/**
 * @brief Base class for all grid levels that work on blocks
 */
template<class Type>
class Blocked : public Level<Type>
{
private:
	/** Number of blocks in each dimension */
	unsigned long m_blocks[MAX_DIMENSIONS];
	
	/** Total number of blocks */
	unsigned long m_totalBlocks;

	/** Number of values in each dimension in one block */
	size_t m_blockSize[MAX_DIMENSIONS];
	
	/** Total number of values in each block */
	size_t m_totalBlockSize;

public:
	/**
	 * @copydoc Level::Level(const mpi::MPIComm&,
	 *  const numa::Numa&, Type&)
	 */
	Blocked(mpi::MPIComm &comm,
			const numa::Numa &numa,
			Type &type)
		: Level<Type>(comm, numa, type),
		  m_totalBlocks(0), m_totalBlockSize(0)
	{
	}

	virtual ~Blocked()
	{
	}
	
protected:
	/**
	 * @copydoc Level::open
	 */
	asagi::Grid::Error open(
			const char* filename,
			const char* varname,
			const int* blockSize,
			int timeDimension,
			grid::ValuePosition valuePos)
	{
		asagi::Grid::Error err = Level<Type>::open(filename, varname, valuePos);
		if (err != asagi::Grid::SUCCESS)
			return err;

		// Check if time dimension is valid
		if (timeDimension >= static_cast<int>(this->dimensions())) {
			logWarning(this->comm().rank()) << "Time dimension is not valid.";
			timeDimension = -1;
		}

		// Set block size in time dimension
		if ((timeDimension >= 0) && (blockSize[timeDimension] == 0)) {
			logDebug(this->comm().rank()) << "Setting block size in time dimension to 1";
			// C to Fortran mapping
			m_blockSize[this->dimensions() - timeDimension - 1] = 1;
		}

		// Set default block size and calculate number of blocks and total block size
		m_totalBlocks = 1;
		m_totalBlockSize = 1;
		for (unsigned int i = 0; i < this->dimensions(); i++) {
			if (blockSize[this->dimensions() - i - 1] == 0)
				// Setting default block size, if not yet set
				m_blockSize[i] = 64;
			else if (blockSize[this->dimensions() - i - 1] < 0)
				// Set block size to maximum
				m_blockSize[i] = this->size(i);
			else
				// C to Fortran mapping
				m_blockSize[i] = blockSize[this->dimensions() - i - 1];

			// A block size large than the dimension does not make any sense
			if (m_blockSize[i] > this->size(i)) {
				logDebug(this->comm().rank()) << "Shrinking block size in dimension"
						<< (this->dimensions()-i-1) << "to" << this->size(i);
				m_blockSize[i] = this->size(i);
			}

			// Total block size
			m_totalBlockSize *= m_blockSize[i];

			// Integer way of rounding up
			m_blocks[i] = (this->size(i) + m_blockSize[i] - 1) / m_blockSize[i];

			// Total number of blocks
			m_totalBlocks *= m_blocks[i];
		}

		return asagi::Grid::SUCCESS;
	}

	/**
	 * @return The number of values in each direction in a block
	 */
	const size_t* blockSize() const
	{
		return m_blockSize;
	}
	
	/**
	 * @return The number of values in direction i in a block
	 */
	size_t blockSize(unsigned int i) const
	{
		return m_blockSize[i];
	}

	/**
	 * @return The number of values in each block
	 */
	unsigned long totalBlockSize() const
	{
		return m_totalBlockSize;
	}
	
	/**
	 * @return The number of blocks in the grid
	 */
	unsigned long totalBlockCount() const
	{
		return m_totalBlocks;
	}

	/**
	 * @return The number of blocks that are stored on this node
	 */
	unsigned long nodeBlockCount() const
	{
		return localBlockCount() * this->numa().totalDomains();
	}

	/**
	 * @return The number of blocks that are stored in this NUMA domain
	 */
	unsigned long localBlockCount() const
	{
		unsigned long cores = this->numa().totalDomains() * this->comm().size();
		return (totalBlockCount() + cores - 1) / cores;
	}

	/**
	 * Calculates the position of <code>block</code> in the grid
	 * 
	 * @param block The global block id
	 * @param[out] pos Position (offset) of the block in each dimension
	 */
	void calcBlockPosition(unsigned long block, size_t *pos) const
	{
		for (int i = this->dimensions()-1; i >= 0; i--) {
			pos[i] = block % m_blocks[i];
			block /= m_blocks[i];
		}
	}

	/**
	 * @return The global block id that stores the value at the given coordinates
	 */
	unsigned long blockByCoords(const size_t *index) const
	{
		unsigned long blockId = 0;

		for (unsigned int i = 0; i < this->dimensions(); i++) {
			blockId *= m_blocks[i];
			blockId += index[i] / m_blockSize[i];
		}

		return blockId;
	}

	/**
	 * @param id A global block id
	 * @return The rank, that stores the block
	 */
	int blockRank(unsigned long id) const
	{
#ifdef ROUND_ROBIN
		return id % this->comm().size();
#else // ROUND_ROBIN
		return id / nodeBlockCount();
#endif // ROUND_ROBIN
	}
	
	/**
	 * @param id A global block id
	 * @return The NUMA domain, that stores the block
	 */
	unsigned int blockDomain(unsigned long id) const
	{
#ifdef ROUND_ROBIN
		return (id / this->comm().size()) % this->numa().totalDomains();
#else // ROUND_ROBIN
		return (id / localBlockCount()) % this->numa().totalDomains();
#endif // ROUND_ROBIN
	}

	/**
	 * @param id Global block id
	 * @return The offset of the block on the corresponding NUMA domain
	 */
	unsigned long blockOffset(unsigned long id) const
	{
#ifdef ROUND_ROBIN
		return id / this->comm().size() / this->numa().totalDomains();
#else // ROUND_ROBIN
		return id % localBlockCount();
#endif // ROUND_ROBIN
	}

	/**
	 * @param id Global block id
	 * @return The offset of the block on the corresponding rank
	 */
	unsigned long blockNodeOffset(unsigned long id) const
	{
		return blockOffset(id) + blockDomain(id) * localBlockCount();
	}

	/**
	 * Converts a local block id to a global block id
	 *
	 * @param id Local block id
	 * @return The corresponding global id
	 */
	unsigned long local2global(unsigned long id) const
	{
		assert(id < localBlockCount());

#ifdef ROUND_ROBIN
		return (id * this->numa().totalDomains() + this->numaDomainId())
				* this->comm().size() + this->comm().rank();
#else // ROUND_ROBIN
		return id + (this->numaDomainId() + this->comm().rank() * this->numa().totalDomains())
				* localBlockCount();
#endif // ROUND_ROBIN
	}
	
	/**
	 * Computes the offset of a value in the block from the coordinates of the value
	 *
	 * @param index The coordinates of the value
	 * @return The offset of the value in the block
	 */
	unsigned long calcOffsetInBlock(const size_t *index)
	{
		unsigned long offset = 0;
		for (unsigned int i = 0; i < this->dimensions(); i++) {
			offset *= blockSize(i);
			offset += index[i] % blockSize(i);
		}

		assert(offset < totalBlockSize());

		return offset;
	}
};

}

}

#endif // GRID_LEVEL_BLOCKED_H

