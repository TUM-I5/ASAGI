/**
 * SPDX-License-Identifier: LGPLv3
 *
 * SPDX-FileCopyrightText: 2012-2013 Sebastian Rettenberger <rettenbs@in.tum.de>
 */

#ifndef TYPES_TYPE_H
#define TYPES_TYPE_H

#include <asagi.h>

#include <cstring>

namespace io {
class NetCdfReader;
}

/**
 * @brief Types specific code
 */
namespace types {

/**
 * Describes the type of the variable stored in each grid cell.
 *
 * This is a base class for arbitrary types with some default implemtentations.
 */
class Type {
  public:
  /**
   * Empty destructor, makes sure constructor
   * of child classes is called
   */
  virtual ~Type() {}

  /**
   * @return The size of the variable
   */
  virtual unsigned int size() const = 0;

#ifndef ASAGI_NOMPI
  /**
   * @return The corresponding MPI_Datatype for this type
   */
  virtual MPI_Datatype getMPIType() const = 0;
#endif // ASAGI_NOMPI

  protected:
  /**
   * Copies the data from <code>data</code> to <code>buf</code>
   */
  static void copy(const void* data, void* buf, unsigned int size) { memcpy(buf, data, size); }
};

} // namespace types

#endif // TYPES_TYPE_H
