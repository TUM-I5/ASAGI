/**
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 * SPDX-FileCopyrightText: 2015 Sebastian Rettenberger <rettenbs@in.tum.de>
 */

#include "mpicomm.h"

/** Lock to protect all MPI calls */
mpi::Lock mpi::MPIComm::mpiLock;
