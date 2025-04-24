/**
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 * SPDX-FileCopyrightText: 2013-2015 Sebastian Rettenberger <rettenbs@in.tum.de>
 */

#ifndef GRID_LEVEL_PASSTHROUGH_H
#define GRID_LEVEL_PASSTHROUGH_H

#include "level.h"
#include "types/type.h"

namespace grid {

namespace level {

/**
 * A simple grid that passes every access directly to the underlying
 * I/O layer.
 */
template <class Type>
class PassThrough : public Level<Type> {
  private:
  /** Buffer for reading one value (on each thread) into memory */
  unsigned char* m_buf;

  public:
  /**
   * @copydoc Level::Level(const mpi::MPIComm&,
   *  const numa::Numa&, Type&)
   */
  PassThrough(mpi::MPIComm& comm, const numa::Numa& numa, Type& type)
      : Level<Type>(comm, numa, type), m_buf(0L) {}

  virtual ~PassThrough() { delete[] m_buf; }

  /**
   * @copydoc Level::open
   */
  asagi::Grid::Error open(const char* filename,
                          const char* varname,
                          const int* blockSize,
                          int timeDimension,
                          unsigned int cacheSize,
                          int cacheHandSpread,
                          grid::ValuePosition valuePos) {
    asagi::Grid::Error err = Level<Type>::open(filename, varname, valuePos);
    if (err != asagi::Grid::SUCCESS)
      return err;

    m_buf = new unsigned char[this->typeSize() * this->numa().totalThreads()];

    return asagi::Grid::SUCCESS;
  }

  /**
   * @copydoc Full::getAt
   */
  template <typename T>
  void getAt(T* buf, const double* pos) {
    this->incCounter(perf::Counter::ACCESS);
    this->incCounter(perf::Counter::FILE);

    // Load one value in each dimension
    size_t index[MAX_DIMENSIONS];
    this->pos2index(pos, index);
    size_t size[MAX_DIMENSIONS];
    std::fill_n(size, MAX_DIMENSIONS, 1);

    // Get the buffer for the current thread
    unsigned char* threadBuf = &m_buf[this->typeSize() * this->numa().threadId()];

    this->type().load(this->inputFile(), index, size, threadBuf);
    this->type().convert(threadBuf, buf);
  }
};

} // namespace level

} // namespace grid

#endif /* GRID_LEVEL_PASSTHROUGH_H */
