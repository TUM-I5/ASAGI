/**
 * @page linux-cluster ASAGI on the LRZ Linux-Cluster
 * 
 * This part describes the installation on the ICE and MPP segments of the LRZ
 * Linux-Cluster. The segments use different MPI versions. Thus, ASAGI compiled
 * on one segment may not work on the other.
 *
 * @section Pre-requirements
 *
 * @subsection additionalmodules Additional modules
 *
 * Load the CMake module and switch to the latest Intel C compiler:
 * @code{.sh}
 * module load cmake
 * module unload ccomp
 * module load ccomp/intel/13.1
 * module unload gcc
 * module load gcc/4.7
 * module load netcdf
 * @endcode
 *
 * The GCC module is required to get support for additional C++11 template
 * libraries.
 *
 * @section lrzcompilation Compilation
 *
 * Selecting Intel compiler and correct NetCDF and MPI libraries can be achieved
 * with the following command:
 * @code{.sh}
 * CXX=icpc CMAKE_PREFIX_PATH="$NETCDF_BASE:$MPI_BASE" \
 *   cmake <path/to/asagi_sources> \
 *   -DCMAKE_INSTALL_PREFIX=<install_dir>
 * @endcode
 */
