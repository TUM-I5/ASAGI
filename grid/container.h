/**
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 * SPDX-FileCopyrightText: 2012-2015 Sebastian Rettenberger <rettenbs@in.tum.de>
 */

#ifndef GRID_CONTAINER_H
#define GRID_CONTAINER_H

#include "asagi.h"

#include "constants.h"
#include "mpi/mpicomm.h"
#include "numa/numa.h"
#include "perf/counter.h"
#include "types/type.h"

namespace grid {

class Grid;

/**
 * A container that stores multiple levels of a grid
 */
class Container {
  private:
  /** The MPI Communicator used for this container */
  mpi::MPIComm& m_comm;

  /** NUMA "communicator" for this container */
  const numa::Numa& m_numa;

  /** Time dimension or -1 if no time dimension exists */
  const int m_timeDimension;

  /** Value Position (cell-centered || vertex-centered) */
  const ValuePosition m_valuePos;

  protected:
  /** The number of dimensions (set by subclasses) */
  unsigned int m_dimensions;
  /** Minimum in each dimension (set by subclasses) */
  double m_min[MAX_DIMENSIONS];
  /** Maximum in each dimension (set by subclasses) */
  double m_max[MAX_DIMENSIONS];

  public:
  /**
   * Constructs a new container
   */
  Container(mpi::MPIComm& comm, const numa::Numa& numa, int timeDimension, ValuePosition valuePos)
      : m_comm(comm), m_numa(numa), m_timeDimension(timeDimension), m_valuePos(valuePos),
        m_dimensions(0) {}

  virtual ~Container() {}

  /**
   * @brief Initialize a level of the container
   *
   * This function is not thread-safe and should only be called by one thread
   *
   * @param filename The name of the file for this level
   * @param varname The variable name in the file
   * @param blockSize Size of the blocks in each dimension
   * @param cacheSize Number of blocks stored in the cache
   * @param cacheHandSpread Difference between the two hands in the LRU algorithm
   * @param level The level that should be initialized
   * @return
   */
  virtual asagi::Grid::Error init(const char* filename,
                                  const char* varname,
                                  const int* blockSize,
                                  unsigned int cacheSize,
                                  int cacheHandSpread,
                                  unsigned int level) = 0;

  /**
   * @return The number of dimensions in the grid
   */
  unsigned int getDimensions() const { return m_dimensions; }

  /**
   * @return The minimum range of the grid in dimension <code>n</code>
   */
  double getMin(unsigned int n) const {
    assert(n < MAX_DIMENSIONS);

    return m_min[n];
  }

  /**
   * @return The maximum range of the grid in dimension <code>n</code>
   */
  double getMax(unsigned int n) const {
    assert(n < MAX_DIMENSIONS);

    return m_max[n];
  }

  /**
   * @return The difference between to grid cells
   */
  virtual double getDelta(unsigned int n, unsigned int level) const = 0;

  /**
   * @see getBuf
   */
  virtual void getByte(unsigned char* buf, const double* pos, unsigned int level = 0) = 0;

  /**
   * @see getBuf
   */
  virtual void getInt(int* buf, const double* pos, unsigned int level = 0) = 0;

  /**
   * @see getBuf
   */
  virtual void getLong(long* buf, const double* pos, unsigned int level = 0) = 0;

  /**
   * @see getBuf
   */
  virtual void getFloat(float* buf, const double* pos, unsigned int level = 0) = 0;

  /**
   * @see getBuf
   */
  virtual void getDouble(double* buf, const double* pos, unsigned int level = 0) = 0;

  /**
   * Write the value at <code>pos</code> into <code>buf</code>.
   *
   * @param buf
   * @param pos
   * @param level
   */
  virtual void getBuf(void* buf, const double* pos, unsigned int level = 0) = 0;

  /**
   * Get a counter for a specific level
   */
  virtual unsigned long getCounter(perf::Counter::CounterType type,
                                   unsigned int level = 0) const = 0;

  protected:
  /**
   * @return The communicator for this container
   */
  const mpi::MPIComm& comm() const { return m_comm; }

  /**
   * @return The communicator for this container
   */
  mpi::MPIComm& comm() { return m_comm; }

  /**
   * @return The NUMA communicator for this container
   */
  const numa::Numa& numa() const { return m_numa; }

  /**
   * @return The time dimension or -1 if not time dimension is set
   */
  int timeDimension() const { return m_timeDimension; }

  /**
   * @return The value position for this container
   */
  ValuePosition valuePosition() const { return m_valuePos; }
};

} // namespace grid

#define CONTAINER_GETVAR_FUNC(N, T)                                                                \
  void get##N(T* buf, const double* pos, unsigned int level = 0) { getAt(buf, pos, level); }
// Use this macro in the subclass to implement all get*(buf, pos, level) functions
// and redirect them to a templatized function getAt<T>(buf, pos, level)
#define CONTAINER_GETVAR                                                                           \
  CONTAINER_GETVAR_FUNC(Byte, unsigned char)                                                       \
  CONTAINER_GETVAR_FUNC(Int, int)                                                                  \
  CONTAINER_GETVAR_FUNC(Long, long)                                                                \
  CONTAINER_GETVAR_FUNC(Float, float)                                                              \
  CONTAINER_GETVAR_FUNC(Double, double)                                                            \
  CONTAINER_GETVAR_FUNC(Buf, void)

#endif // GRID_CONTAINER_H
