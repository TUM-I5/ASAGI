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

#ifndef GRID_LEVEL_FULL_H
#define GRID_LEVEL_FULL_H

#include "blocked.h"
#include "allocator/default.h"

namespace grid
{

namespace level
{

/**
 * This grid loads all (local) blocks into memory at initialization.
 * Neither does this class change the blocks nor does it fetch new blocks.
 * If you try to access values of a non-local block, the behavior is
 * undefined.
 * 
 * If compiled without MPI, all blocks are local.
 */
template<class Type, class Allocator>
class Full : public Blocked<Type>
{
private:
	/** Local data cache */
	unsigned char* m_data;

public:
	/**
	 * @copydoc Blocked::Blocked
	 */
	Full(const mpi::MPIComm &comm,
			const numa::Numa &numa,
			Type &type)
		: Blocked<Type>(comm, numa, type),
		  m_data(0L)
	{
	}

	virtual ~Full()
	{
		if (&this->numa() && this->numaDomainId() == 0)
			this->numa().template free<Allocator>(m_data);
	}
	
	/**
	 * @copydoc Blocked::open
	 */
	asagi::Grid::Error open(
		const char* filename,
		const char* varname,
		const unsigned int* blockSize,
		int timeDimension,
		unsigned int cacheSize,
		int cacheHandSpread,
		grid::ValuePosition valuePos)
	{
		asagi::Grid::Error err = Blocked<Type>::open(filename, varname,
				blockSize, timeDimension, valuePos);
		if (err != asagi::Grid::SUCCESS)
			return err;

		// Allocate the memory
		err = this->numa().template allocate<Allocator>(
				this->typeSize() * this->totalBlockSize() * this->localBlockCount(),
				m_data);
		if (err != asagi::Grid::SUCCESS)
			return err;

		// Load the blocks from the file
		for (unsigned long i = 0; i < this->localBlockCount(); i++) {
			if (this->local2global(i) >= this->totalBlockCount())
				// Last process(es) may control less blocks
				break;

			// Get coordinates of the block
			size_t blockPos[MAX_DIMENSIONS];
			this->calcBlockPosition(this->local2global(i), blockPos);

			// Get coordinates of the first value in the block
			for (unsigned char j = 0; j < this->dimensions(); j++)
				blockPos[j] *= this->blockSize(j);

			// Load the block
			this->type().load(this->inputFile(),
				blockPos, this->blockSize(),
				&m_data[this->typeSize() * this->totalBlockSize() * i]);
		}

		this->closeInputFile();

		return asagi::Grid::SUCCESS;
	}
	
	/**
	 * Writes the value at position <code>pos</code> into the buffer
	 * <code>buf</code>.
	 */
	template<typename T>
	void getAt(T* buf, const double* pos)
	{
		this->incCounter(perf::Counter::ACCESS);

		// Get the index from the position
		size_t index[MAX_DIMENSIONS];
		this->pos2index(pos, index);

		// Get block id from the index
		unsigned long globalBlockId = this->blockByCoords(index);

		assert(this->blockRank(globalBlockId) == this->comm().rank());
		assert(this->blockDomain(globalBlockId) == this->numaDomainId());

		// The offset of the block
		unsigned long localBlockId = this->blockOffset(globalBlockId);
		assert(localBlockId < this->localBlockCount());

		// Compute the offset of the value in the block
		unsigned long offset = this->calcOffsetInBlock(index);

		// Finally, we fill the buffer
		this->type().convert(
				&m_data[this->typeSize() *
						(localBlockId * this->totalBlockSize() + offset)],
				buf);
	}

protected:
	/**
	 * @return A pointer to the blocks
	 */
	unsigned char* data()
	{
		return m_data;
	}
};

/** Full level with default allocator */
template<class Type>
using FullDefault = Full<Type, allocator::Default>;

}

}

#endif // GRID_LEVEL_FULL_H
