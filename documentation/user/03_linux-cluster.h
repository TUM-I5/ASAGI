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
 * module load ccomp/intel/12.1
 * module load gcc/4.5
 * @endcode
 *
 * The GCC module is required to get support for additional C++11 template
 * libraries.
 *
 * @subsection NetCDF
 *
 * The NetCDF libraries available on the Linux-Cluster do not support the new
 * C++4 interface. The latest NetCDF version also requires a newer HDF5 library
 * than available on the Linux-Cluster. Thus, first download HDF5
 * (http://www.hdfgroup.org/HDF5/) and compile it:
 * @code{.sh}
 * CC=icc ./configure --prefix=<install_dir>
 * make
 * make install
 * @endcode
 *
 * Now, you can compile NetCDF:
 * @code{.sh}
 * CC=icc CPPFLAGS=-I<install_dir>/include \
 *   LDFLAGS=-L<install_dir>/lib \
 *   ./configure --enable-shared --enable-cxx-4 \
 *   --prefix=<install_dir>
 * make
 * make install
 * @endcode
 *
 * @section lrzcompilation Compilation
 *
 * Selecting Intel compiler and correct NetCDF and MPI libraries can be achieved
 * with the following command:
 * @code{.sh}
 * CXX=icpc CMAKE_PREFIX_PATH="<install_dir>:$MPI_BASE" \
 *   cmake <path/to/asagi_sources> \
 *   -DCMAKE_INSTALL_PREFIX=<install_dir>
 * @endcode
 */