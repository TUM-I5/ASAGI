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
 * @copyright 2015 Sebastian Rettenberger <rettenbs@in.tum.de>
 *
 * @page Troubleshooting Troubleshooting
 * 
 * @section cmakempi CMake does not find MPI
 * 
 * On some plattforms, CMake has problems finding MPI. Try to set the
 * environment variable <code>CMAKE_PREFIX_PATH</code> (see section @ref build)
 * or select the MPI compiler before running CMake by setting the enviroment
 * variable <code>CXX</code>.
 *
 * @section hang The program hangs
 *
 * @subsection intelmpi Intel MPI
 * Due to a bug (http://software.intel.com/en-us/forums/showthread.php?t=103456)
 * in the Intel MPI library (version 4.0 update 3 and probably earlier versions)
 * the remote memory access in ASAGI does not work properly. This only happens
 * when fabric is set to "ofa" or "shm:ofa". Selecting a different fabric by
 * changing the environment variable "I_MPI_FABRICS" solves the problem.
 *
 * @subsection OpenMPI
 * OpenMPI windows also have a bug that might create deadlocks if more than one
 * thread is used.
 *
 * The bug could be reproduced with the following example with OpenMPI 1.8.7:
 * @code{.cpp}
 * #include <mpi.h>
 * #include <unistd.h>
 * #include <iostream>
 *
 * int main(int argc, char* argv[]) {
 *   int provided;
 *   MPI_Init_thread(&argc, &argv, MPI_THREAD_SERIALIZED, &provided);
 *   //MPI_Init(&argc, &argv);
 *
 *   int rank;
 *   MPI_Comm_rank(MPI_COMM_WORLD, &rank);
 *
 *   MPI_Win window;
 *   int* data;
 *   MPI_Alloc_mem(sizeof(int), MPI_INFO_NULL, &data);
 *   *data = 0;
 *
 *   MPI_Win_create(data,
 *     sizeof(int),
 *     sizeof(int),
 *     MPI_INFO_NULL,
 *     MPI_COMM_WORLD, &window);
 *
 *   if (rank == 0) {
 *     sleep(3);
 *
 *     std::cout << "Start rank 0" << std::endl;
 *
 *     MPI_Win_lock(MPI_LOCK_SHARED, 1, 0, window);
 *     int mydata = 1;
 *     MPI_Put(&mydata, 1, MPI_INT, 1, 0, 1, MPI_INT, window);
 *     MPI_Win_unlock(1, window);
 *
 *     char signal = 0;
 *     MPI_Send(&signal, 1, MPI_CHAR, 1, 0, MPI_COMM_WORLD);
 *
 *     std::cout << "End rank 0" << std::endl;
 *   } else {
 *     std::cout << "Start rank 1" << std::endl;
 *
 *     char signal;
 *     MPI_Recv(&signal, 1, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
 *
 *     std::cout << "End rank 1" << std::endl;
 *   }
 *
 *   MPI_Win_free(&window);
 *   MPI_Free_mem(data);
 *
 *   MPI_Finalize();
 *
 *   return 0;
 * }
 * @endcode
 *
 * This bug seems to be fixed in version 1.10.0.
 *
 * @section mappedmemory The program fails with "PMPI_Win_create: Assertion
 *  'winptr-\>lock_table[i]' failed" or "function:MPI_WIN_LOCK, Invalid win argument"
 *
 * The SGI Message Passing Toolkit uses a special mapped memory for one-sided
 * communication. For large grids the default size of mapped memory may be too
 * small. It is possible to increase the size by setting the environment
 * variable <code>MPI_MAPPED_HEAP_SIZE</code>.
 */
