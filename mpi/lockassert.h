/**
 * SPDX-License-Identifier: LGPLv3
 *
 * SPDX-FileCopyrightText: 2015 Sebastian Rettenberger <rettenbs@in.tum.de>
 */

// Work around a bug in OpenMPI 1.10
// See: https://github.com/open-mpi/ompi-release/pull/700
#if OMPI_MAJOR_VERSION == 1 && OMPI_MINOR_VERSION == 10 && OMPI_RELEASE_VERSION == 0
#define ASAGI_MPI_MODE_NOCHECK 0
#else // OMPI_MAJOR_VERSION == 1 && OMPI_MINOR_VERSION == 10 && OMPI_RELEASE_VERSION == 0
#define ASAGI_MPI_MODE_NOCHECK MPI_MODE_NOCHECK
#endif // OMPI_MAJOR_VERSION == 1 && OMPI_MINOR_VERSION == 10 && OMPI_RELEASE_VERSION == 0
