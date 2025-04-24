/**
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 * SPDX-FileCopyrightText: 2015 Sebastian Rettenberger <rettenbs@in.tum.de>
 */

#ifndef MPI_NOMPICOMM_H
#define MPI_NOMPICOMM_H

namespace mpi {

/**
 * MPI Communicator if MPI is disabled
 */
class NoMPIComm {
  public:
  NoMPIComm() {}

  virtual ~NoMPIComm() {}

  /**
   * @return 0
   */
  int rank() const { return 0; }

  /**
   * @return 1
   */
  int size() const { return 1; }

  /**
   * Does nothing
   */
  void barrier() const {}
};

} // namespace mpi

#endif // MPI_NOMPICOMM_H
