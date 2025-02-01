/**
 * SPDX-License-Identifier: LGPLv3
 *
 * SPDX-FileCopyrightText: 2015 Sebastian Rettenberger <rettenbs@in.tum.de>
 */

#include "mpicomm.h"

/** Lock to protect all MPI calls */
mpi::Lock mpi::MPIComm::mpiLock;
