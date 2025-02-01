/**
 * SPDX-License-Identifier: LGPLv3
 *
 * SPDX-FileCopyrightText: 2012-2015 Sebastian Rettenberger <rettenbs@in.tum.de>
 */

#ifndef GRID_SIMPLECONTAINER_H
#define GRID_SIMPLECONTAINER_H

#include <vector>

#include "typedcontainer.h"

#include "utils/logger.h"

namespace grid {

/**
 * Simple container that stores the whole grid for each level.
 */
template <class Level, class Type>
class SimpleContainer : public TypedContainer<Type> {
  private:
  /** All grids we control */
  std::vector<Level> m_levels;

  public:
  /**
   * @copydoc TypedContainer::TypedContainer
   */
  SimpleContainer(mpi::MPIComm& comm,
                  const numa::Numa& numa,
                  Type& type,
                  int timeDimension,
                  ValuePosition valuePos)
      : TypedContainer<Type>(comm, numa, type, timeDimension, valuePos) {}

  virtual ~SimpleContainer() { m_levels.clear(); }

  asagi::Grid::Error init(const char* filename,
                          const char* varname,
                          const int* blockSize,
                          unsigned int cacheSize,
                          int cacheHandSpread,
                          unsigned int level) {
    if (m_levels.size() <= level)
      m_levels.resize(level + 1, Level(this->comm(), this->numa(), this->type()));

    asagi::Grid::Error err = m_levels[level].open(filename,
                                                  varname,
                                                  blockSize,
                                                  this->timeDimension(),
                                                  cacheSize,
                                                  cacheHandSpread,
                                                  this->valuePosition());
    if (err != asagi::Grid::SUCCESS)
      return err;

    if (level == 0) {
      // Set dimensions
      this->m_dimensions = m_levels[0].dimensions();

      // Set min/max/...
      for (unsigned int i = 0; i < m_levels[0].dimensions(); i++) {
        this->m_min[i] = m_levels[0].min(i);
        this->m_max[i] = m_levels[0].max(i);
      }
    }

    return asagi::Grid::SUCCESS;
  }

  double getDelta(unsigned int n, unsigned int level) const {
    assert(n < MAX_DIMENSIONS);
    assert(level < m_levels.size());

    return m_levels[level].delta(n);
  }

  CONTAINER_GETVAR

  /**
   * Template function that is called by {@link getFloat}, etc.
   * This function calls the correct <code>getAt</code> from the
   * level.
   */
  template <typename T>
  void getAt(T* buf, const double* pos, unsigned int level = 0) {
    assert(level < m_levels.size());

    m_levels[level].getAt(buf, pos);
  }

  unsigned long getCounter(perf::Counter::CounterType type, unsigned int level = 0) const {
    assert(level < m_levels.size());

    return m_levels[level].getCounter(type);
  }
};

} // namespace grid

#endif // GRID_SIMPLECONTAINER_H
