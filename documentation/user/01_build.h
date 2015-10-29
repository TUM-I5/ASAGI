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
 * @page build Building and Installing ASAGI
 * 
 * @section Pre-requirements
 *
 * @subsection Compiler
 * ASAGI requires at least GCC 4.7 or Intel Compiler 12.1
 *
 * @subsection MPI
 * ASAGI makes use of the RMA (Remote Memory Access) API of the MPI-2 standard
 * to transfer data. An MPI library that supports the new standard is required.
 *
 * @subsection NetCDF
 * 
 * ASAGI uses the NetCDF library (http://www.unidata.ucar.edu/software/netcdf/)
 * to load data files.
 *
 * @subsection pthreads POSIX Threads (optional)
 *
 * The PThreads library is required if ASAGI is compiled with NUMA support.
 * If available it is also used to guaranty thread safety as an alternative
 * to <code>std::mutex</code>.
 *
 * @subsection libnuma NUMA policy library (optional)
 *
 * ASASGI uses the NUMA library to detect NUMA domains. This library is required
 * if ASAGI should be compiled with NUMA support.
 *
 * @section Compilation
 *
 * To generate the Makefiles, CMake is used. For CMake it is recommend to keep
 * source and build directory apart:
 * @code{.sh}
 * mkdir build
 * cd build
 * cmake <path/to/asagi_sources>
 * @endcode
 *
 * Several environment variables affect the behavior of CMake. They must be set
 * before running "cmake".
 *
 * @li @b Compiler The compiler can be selected by setting <code>CC</code>
 *  (C compiler), <code>CXX</code> (C++ compiler) and <code>FC</code> (Fortran
 *  compiler) environment variables. C and Fortran compiler are only required
 *  for C and Fortran examples and tests.
 * @li @b Libraries The <code>CMAKE_PREFIX_PATH</code> is used when searching
 *  for the MPI, NetCDF, POSIX Threads and NUMA library. If NetCDF was configured
 *  with <code>--prefix=\<install_dir\></code> for example, set
 *  <code>CMAKE_PREFIX_PATH=\<install_dir\></code>.
 *
 * Besides the environment variables, you can change the behavior by setting
 * internal CMake variables. They can be configured by adding one ore more
 * <code>-D\<variable\>=\<value\></code> options when running "cmake". These
 * variables can also be changed later with the following command:
 * @code{.sh}
 * ccmake <path/to/asagi_build>
 * @endcode
 *
 * The important variables are listed below. Most of the variables are ASAGI
 * specific and will not work with other CMake projects.
 * @li <b>CMAKE_BUILD_TYPE = Debug | Release</b> When set to "Debug", additional
 *  run-time checks are enabled as well as debug messages.
 *  <code>[Release]</code>
 * @li <b>CMAKE_INSTALL_PREFIX</b> Installation directory for ASAGI.
 *  <code>[/usr/local/]</code>
 * @li <b>SHARED_LIB = ON | OFF</b> Build shared library. <code>[ON]</code>
 * @li <b>STATIC_LIB = ON | OFF</b> Build static library. <code>[OFF]</code>
 * @li <b>FORTRAN_SUPPORT = ON | OFF</b> Compile with Fortran support.
 *  <code>[ON]</code>
 * @li <b>MAX_DIMENSIONS</b> Maximum number of dimensions supported by ASAGI
 *  <code>[4]</code>
 * @li <b>THREADSAFE = ON | OFF</b> If enabled all ASAGI functions are
 *  thread-safe. This is required, for example, if ASAGI is used in hybrid
 *  MPI/OpenMP programs. <code>[ON]</code>
 * @li <b>THREADSAFE_COUNTER = ON | OFF</b> Make access counters thread-safe.
 *  This may lead to a performance loss but makes sure, counters are accurate.
 *  <code>[OFF]</code>
 * @li <b>THREADSAFE_MPI = ON | OFF</b> Make MPI calls thread-safe. This is
 *  required if the MPI library is not thread-safe by itself. <code>[ON]</code>
 * @li <b>NOMPI = ON | OFF</b> Do not compile with MPI support. All algorithms
 *  that require MPI communication will be disabled. <code>[OFF]</code>
 * @li <b>NONUMA = ON | OFF</b> Do not compile with NUMA support. All intra-node
 *  communications will be turned off. <code>[OFF]</code>
 * @li <b>MPI3 = ON | OFF</b> Compile MPI-3 functionality. Some features are
 *  only available when this flag is switched on. <code>[ON]</code>
 * @li <b>TESTS = ON | OFF</b> Compile tests. <code>[OFF]</code>
 * @li <b>EXAMPLES = ON | OFF</b> Compile example programs. <code>[OFF]</code>
 *
 * @section Tests
 *
 * If you have enabled the tests, you can run them with the following command:
 * @code{.sh}
 * make test
 * @endcode
 *
 * @section Installation
 *
 * To install ASAGI simply run:
 * @code{.sh}
 * make install
 * @endcode
 * 
 * This will install the (static and/or shared) library as well as the header
 * files. If pkg-config was found, this  command will also install a pkg-config
 * configuration file for ASAGI in <code>CMAKE_INSTALL_PREFIX/lib/pkgconfig</code>
 *
 * You can install ASAGI with and without MPI support on your system. The version
 * with MPI will be called <code>asagi</code> and the version without MPI
 * <code>asagi_nompi</code>. Use the same include file for both libraries, but
 * if you do not compile your program with MPI, make sure to define
 * <code>ASAGI_NOMPI</code> before including the ASAGI header:
 * @code{.c}
 * #define ASAGI_NOMPI
 * @endcode
 */
