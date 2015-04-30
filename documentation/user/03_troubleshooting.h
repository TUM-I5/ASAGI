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
 * Due to a bug (http://software.intel.com/en-us/forums/showthread.php?t=103456)
 * in the Intel MPI library (version 4.0 update 3 and probably earlier versions)
 * the remote memory access in ASAGI does not work properly. This only happens
 * when fabric is set to "ofa" or "shm:ofa". Selecting a different fabric by
 * changing the environment variable "I_MPI_FABRICS" solves the problem.
 *
 * @section mappedmemory The program fails with "PMPI_Win_create: Assertion
 *  'winptr-\>lock_table[i]' failed" or "function:MPI_WIN_LOCK, Invalid win argument"
 *
 * The SGI Message Passing Toolkit uses a special mapped memory for one-sided
 * communication. For large grids the default size of mapped memory may be too
 * small. It is possible to increase the size by setting the environment
 * variable <code>MPI_MAPPED_HEAP_SIZE</code>.
 */
