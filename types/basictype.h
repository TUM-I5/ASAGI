/**
 * SPDX-License-Identifier: LGPLv3
 *
 * SPDX-FileCopyrightText: 2012-2015 Sebastian Rettenberger <rettenbs@in.tum.de>
 */

#ifndef TYPES_BASICTYPE_H
#define TYPES_BASICTYPE_H

#include "type.h"
#include "io/netcdfreader.h"

namespace types {

/**
 * Implementation for basic types like int, long, float, double
 */
template <typename T>
class BasicType : public Type {
  public:
  /**
   * Check compatibility of the input file with this type.
   */
  asagi::Grid::Error check(const io::NetCdfReader& file) { return asagi::Grid::SUCCESS; }

  unsigned int size() const { return sizeof(T); }

  /**
   * Loads a block form the netCDF file into the buffer
   */
  void load(io::NetCdfReader& file, const size_t* offset, const size_t* size, void* buf) const {
    file.getBlock<T>(buf, offset, size);
  }

#ifndef ASAGI_NOMPI
  virtual MPI_Datatype getMPIType() const;
#endif // ASAGI_NOMPI

  /**
   * The value is copied from data to buf, doing transformations between
   * basic types.
   */
  template <typename B>
  void convert(const void* data, B* buf) const {
    *buf = *static_cast<const T*>(data);
  }

  /**
   * Uses the "no-conversion" function for void pointers
   */
  void convert(const void* data, void* buf) const { Type::copy(data, buf, sizeof(T)); }
};

#ifndef ASAGI_NOMPI
template <>
inline MPI_Datatype BasicType<unsigned char>::getMPIType() const {
  return MPI_BYTE;
}

template <>
inline MPI_Datatype BasicType<int>::getMPIType() const {
  return MPI_INT;
}

template <>
inline MPI_Datatype BasicType<long>::getMPIType() const {
  return MPI_LONG;
}

template <>
inline MPI_Datatype BasicType<float>::getMPIType() const {
  return MPI_FLOAT;
}

template <>
inline MPI_Datatype BasicType<double>::getMPIType() const {
  return MPI_DOUBLE;
}
#endif // ASAGI_NOMPI

} // namespace types

#endif // TYPES_BASICTYPE_H
