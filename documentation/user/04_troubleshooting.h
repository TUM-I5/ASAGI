/**
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
