/**
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
