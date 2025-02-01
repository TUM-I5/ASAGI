/**
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 * SPDX-FileCopyrightText: 2012-2013 Sebastian Rettenberger <rettenbs@in.tum.de>
 */

#include "structtype.h"

/**
 * Creates a struct type
 *
 * @relates types::StructType
 */
types::Type* types::createStruct(unsigned int count,
                                 unsigned int blockLength[],
                                 unsigned long displacements[],
                                 asagi::Grid::Type types[]) {
  assert(count >= 1);
  assert(displacements[0] == 0);

  switch (types[0]) {
  case asagi::Grid::BYTE:
    return new types::StructType<unsigned char>(count, blockLength, displacements, types);
  case asagi::Grid::INT:
    return new types::StructType<int>(count, blockLength, displacements, types);
  case asagi::Grid::LONG:
    return new types::StructType<long>(count, blockLength, displacements, types);
  case asagi::Grid::FLOAT:
    return new types::StructType<float>(count, blockLength, displacements, types);
  case asagi::Grid::DOUBLE:
    return new types::StructType<double>(count, blockLength, displacements, types);
  }

  assert(false);

  return 0L;
}
