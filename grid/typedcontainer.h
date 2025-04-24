/**
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 * SPDX-FileCopyrightText: 2015 Sebastian Rettenberger <rettenbs@in.tum.de>
 */

#ifndef GRID_TYPEDCONTAINER_H
#define GRID_TYPEDCONTAINER_H

#include "container.h"

namespace grid {

/**
 * A container with a specific element type
 */
template <class Type>
class TypedContainer : public Container {
  private:
  /**
   * The type of values we save in the grid.
   * This class implements all type specific operations.
   */
  Type& m_type;

  public:
  /**
   * @copydoc Container::Container
   */
  TypedContainer(mpi::MPIComm& comm,
                 const numa::Numa& numa,
                 Type& type,
                 int timeDimension,
                 ValuePosition valuePos)
      : Container(comm, numa, timeDimension, valuePos), m_type(type) {}

  virtual ~TypedContainer() {}

  /**
   * @return The type for this container
   */
  const Type& type() const { return m_type; }

  /**
   * @copydoc const Type& type() const
   */
  Type& type() { return m_type; }
};

} // namespace grid

#endif // GRID_TYPEDCONTAINER_H
