/**
 * @file
 *  This file is part of ASAGI.
 *
 *  ASAGI is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as
 *  published by the Free Software Foundation, either version 3 of
 *  the License, or  (at your option) any later version.
 *
 *  ASAGI is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with ASAGI.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Diese Datei ist Teil von ASAGI.
 *
 *  ASAGI ist Freie Software: Sie koennen es unter den Bedingungen
 *  der GNU Lesser General Public License, wie von der Free Software
 *  Foundation, Version 3 der Lizenz oder (nach Ihrer Option) jeder
 *  spaeteren veroeffentlichten Version, weiterverbreiten und/oder
 *  modifizieren.
 *
 *  ASAGI wird in der Hoffnung, dass es nuetzlich sein wird, aber
 *  OHNE JEDE GEWAEHELEISTUNG, bereitgestellt; sogar ohne die implizite
 *  Gewaehrleistung der MARKTFAEHIGKEIT oder EIGNUNG FUER EINEN BESTIMMTEN
 *  ZWECK. Siehe die GNU Lesser General Public License fuer weitere Details.
 *
 *  Sie sollten eine Kopie der GNU Lesser General Public License zusammen
 *  mit diesem Programm erhalten haben. Wenn nicht, siehe
 *  <http://www.gnu.org/licenses/>.
 *
 * @copyright 2013-2015 Sebastian Rettenberger <rettenbs@in.tum.de>
 */

#ifndef ALLOCATOR_MPIALLOC_H
#define ALLOCATOR_MPIALLOC_H

#ifdef ASAGI_NOMPI
#include "default.h"
#else // ASAGI_NOMPI
#include "asagi.h"

#include "mpi/mpicomm.h"
#endif // ASAGI_NOMPI

namespace allocator
{

#ifdef ASAGI_NOMPI
/** Use the default allocator if MPI is not available */
typedef Default MPIAlloc;
#else // ASAGI_NOMPI

/**
 * This allocator uses MPI mechanisms to allocate/free memory
 */
class MPIAlloc
{
public:
	/**
	 * @copydoc Default::allocate
	 */
	template<typename T>
	static asagi::Grid::Error allocate(size_t size, T* &ptr)
	{
		std::lock_guard<mpi::Lock> lock(mpi::MPIComm::mpiLock);

		if (MPI_Alloc_mem(size * sizeof(T), MPI_INFO_NULL, &ptr) != MPI_SUCCESS)
			return asagi::Grid::MPI_ERROR;

		return asagi::Grid::SUCCESS;
	}

	/**
	 * @copydoc Default::free
	 */
	template<typename T>
	static void free(T *ptr)
	{
		if (ptr) {
			std::lock_guard<mpi::Lock> lock(mpi::MPIComm::mpiLock);
			MPI_Free_mem(ptr);
		}
	}
};

#endif // ASAGI_NOMPI

}

#endif // ALLOCATOR_MPIALLOC_H
