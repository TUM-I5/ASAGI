/**
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 * SPDX-FileCopyrightText: 2012-2015 Sebastian Rettenberger <rettenbs@in.tum.de>
 */

#ifndef GRID_LEVEL_FULL_H
#define GRID_LEVEL_FULL_H

#include "blocked.h"
#include "allocator/default.h"

namespace grid {

namespace level {

/**
 * This grid loads all (local) blocks into memory at initialization.
 * Neither does this class change the blocks nor does it fetch new blocks.
 * If you try to access values of a non-local block, the behavior is
 * undefined.
 *
 * If compiled without MPI, all blocks are local.
 */
template <class Type, class Allocator>
class Full : public Blocked<Type> {
  private:
  /** Local data cache */
  unsigned char* m_data;

  public:
  /**
   * @copydoc Blocked::Blocked
   */
  Full(mpi::MPIComm& comm, const numa::Numa& numa, Type& type)
      : Blocked<Type>(comm, numa, type), m_data(0L) {}

  virtual ~Full() {
    if (&this->numa() && this->numaDomainId() == 0)
      this->numa().template free<Allocator>(m_data);
  }

  /**
   * @copydoc Blocked::open
   */
  asagi::Grid::Error open(const char* filename,
                          const char* varname,
                          const int* blockSize,
                          int timeDimension,
                          unsigned int cacheSize,
                          int cacheHandSpread,
                          grid::ValuePosition valuePos) {
    asagi::Grid::Error err =
        Blocked<Type>::open(filename, varname, blockSize, timeDimension, valuePos);
    if (err != asagi::Grid::SUCCESS)
      return err;

    // Allocate the memory
    err = this->numa().template allocate<Allocator>(
        this->typeSize() * this->totalBlockSize() * this->localBlockCount(), m_data);
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
                        blockPos,
                        this->blockSize(),
                        &m_data[this->typeSize() * this->totalBlockSize() * i]);
    }

    this->closeInputFile();

    return asagi::Grid::SUCCESS;
  }

  /**
   * Writes the value at position <code>pos</code> into the buffer
   * <code>buf</code>.
   */
  template <typename T>
  void getAt(T* buf, const double* pos) {
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
        &m_data[this->typeSize() * (localBlockId * this->totalBlockSize() + offset)], buf);
  }

  protected:
  /**
   * @return A pointer to the blocks
   */
  unsigned char* data() { return m_data; }
};

/** Full level with default allocator */
template <class Type>
using FullDefault = Full<Type, allocator::Default>;

} // namespace level

} // namespace grid

#endif // GRID_LEVEL_FULL_H
