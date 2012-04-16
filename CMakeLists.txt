project( ASAGI )

# Version
cmake_minimum_required( VERSION 2.8 )

##### User configuration #####
option( STATIC_LIB "Compile asagi as static library" OFF )
option( THREADSAFETY "Enable support for multiple threads on each node" ON )
option( FORTRAN_SUPPORT "Enable Fortran support" ON )
option( EXAMPLES "Compile examples" OFF )
option( TESTS "Test library" OFF )
option( NO_MPI "Compile without MPI" OFF )
option( ROUND_ROBIN "Distribute blocks round robin on nodes (internal use)" ON )

##### Default build type #####
if( NOT CMAKE_BUILD_TYPE )
	set( CMAKE_BUILD_TYPE "Release" CACHE STRING
		"Choose the type of build, options are: Debug Release" FORCE)
endif( NOT CMAKE_BUILD_TYPE )

##### Files #####
# Lib Sources
set( LibSources
	asagi.cpp
	asagi_f90.cpp
	grid.cpp
	gridcontainer.cpp
	nompigrid.cpp )
if( NOT NO_MPI )
	set( LibSources ${LibSources}
		largegrid.cpp
		simplegrid.cpp )
endif( NOT NO_MPI )

# Include files for other programs
set( InterfaceHeaders
	${CMAKE_SOURCE_DIR}/include/asagi.h
	${CMAKE_SOURCE_DIR}/include/asagi.inc )

##### Other configuration #####
# Additional cmake modules
set( CMAKE_MODULE_PATH
	${CMAKE_MODULE_PATH}
	${CMAKE_SOURCE_DIR}/CMake/Modules/ )

# Enable warnings
add_definitions( -Wall )

# Set c++11 standard
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x" )

# Enable fortran
if( FORTRAN_SUPPORT )
	# Call this before searching for mpi
	enable_language( Fortran )
endif()

# Thread-safety
if( THREADSAFETY )
	add_definitions( -DTHREADSAFETY )
endif()

# Use MPI
if( NO_MPI )
	add_definitions( -DASAGI_NOMPI )
endif( NO_MPI )

# Round robin
if( ROUND_ROBIN )
	add_definitions( -DROUND_ROBIN )
endif( ROUND_ROBIN )

if( NOT NO_MPI )
	# Find MPI
	find_package( MPI REQUIRED )
	if( MPI_LIBRARY )
		include_directories( ${MPI_C_INCLUDE_PATH} SYSTEM )
	else()
		#Workaround for stupid mpicc/mpicxx
		get_filename_component( MPI_BASE ${MPI_C_COMPILER} PATH )
		set( MPI_BASE ${MPI_BASE}/.. )
		include_directories( ${MPI_BASE}/include SYSTEM )
		link_directories( ${MPI_BASE}/lib )
	endif()
	set( CMAKE_C_COMPILER ${MPI_C_COMPILER} )
	set( CMAKE_CXX_COMPILER ${MPI_CXX_COMPILER} )
	set( CMAKE_Fortran_COMPILER ${MPI_Fortran_COMPILER} )
endif( NOT NO_MPI )

# Find netcdf
set( NETCDF_CXX4 "YES" )
find_package( NetCDF REQUIRED )
include_directories( ${NETCDF_INCLUDES} SYSTEM )

# Find png
find_package( PNG )
if( PNG_FOUND )
	add_definitions( -DPNG_ENABLED )
endif()

# Add library include files for all exectuables
include_directories( ${CMAKE_SOURCE_DIR}/include SYSTEM )

# For sub directories
include_directories( ${CMAKE_SOURCE_DIR} )

# Add static libaries
add_subdirectory( blocks )
add_subdirectory( debug )
add_subdirectory( fortran )
add_subdirectory( io )
if( NOT NO_MPI )
	add_subdirectory( mpi )
endif( NOT NO_MPI )
add_subdirectory( types )

# Documentation
add_subdirectory( documentation )

# Test and examples do not yet work without MPI
if( NOT NO_MPI )
	if( TESTS )
		# Add tests
		enable_testing()
		add_subdirectory( tests )
	endif()

	if( EXAMPLES )
		# Add examples
		add_subdirectory( examples )
	endif()
endif( NOT NO_MPI )

##### Excecutables #####
# Library
if( STATIC_LIB )
	add_library( asagi
		${LibSources} )
else()
	add_library( asagi SHARED
		${LibSources} )
endif()
target_link_libraries( asagi
	blocks
	io
	types )
if( NOT NO_MPI )
	target_link_libraries( asagi mpi )
endif( NOT NO_MPI )
