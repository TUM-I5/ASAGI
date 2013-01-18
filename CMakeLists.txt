cmake_minimum_required( VERSION 2.8 )

# ASAGI
project( ASAGI )

set( ASAGI_VERSION_MAJOR 0 )
set( ASAGI_VERSION_MINOR 2 )

##### User configuration #####
option( STATIC_LIB "Compile asagi as static library" OFF )
option( THREADSAFETY "Enable support for multiple threads on each node" ON )
option( FORTRAN_SUPPORT "Enable Fortran support" ON )
option( EXAMPLES "Compile examples" OFF )
option( TESTS "Test library" OFF )
option( NOMPI "Compile without MPI" OFF )
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
	asagi_f90.cpp )

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

# Order include path (solves issues for developers when asagi.{h,inc}
# is already installed)
set( CMAKE_INCLUDE_DIRECTORIES_PROJECT_BEFORE ON )

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
if( NOMPI )
	add_definitions( -DASAGI_NOMPI )
endif( NOMPI )

# Round robin
if( ROUND_ROBIN )
	add_definitions( -DROUND_ROBIN )
endif( ROUND_ROBIN )

if( NOT NOMPI )
	# Find MPI
	find_package( MPI REQUIRED )
	if( MPI_LIBRARY )
		include_directories( SYSTEM ${MPI_C_INCLUDE_PATH} )
	else()
		#Workaround for stupid mpicc/mpicxx
		get_filename_component( MPI_BASE ${MPI_C_COMPILER} PATH )
		set( MPI_BASE ${MPI_BASE}/.. )
		include_directories( SYSTEM ${MPI_BASE}/include )
		link_directories( ${MPI_BASE}/lib )
	endif()
	set( CMAKE_C_COMPILER ${MPI_C_COMPILER} )
	set( CMAKE_CXX_COMPILER ${MPI_CXX_COMPILER} )
	set( CMAKE_Fortran_COMPILER ${MPI_Fortran_COMPILER} )
endif( NOT NOMPI )

# Find netcdf
set( NETCDF_CXX4 "YES" )
find_package( NetCDF REQUIRED )
include_directories( SYSTEM ${NETCDF_INCLUDES} )

# Find png
find_package( PNG )
if( PNG_FOUND )
	add_definitions( -DPNG_ENABLED )
endif()

# Add library include files for all exectuables
include_directories( ${CMAKE_SOURCE_DIR}/include )

# For sub directories
include_directories( ${CMAKE_SOURCE_DIR} )

# Add static libaries
add_subdirectory( allocator )
add_subdirectory( blocks )
add_subdirectory( debug )
add_subdirectory( fortran )
add_subdirectory( grid )
add_subdirectory( io )
add_subdirectory( perf )
add_subdirectory( utils )
if( NOT NOMPI )
	add_subdirectory( mpi )
endif( NOT NOMPI )
add_subdirectory( types )

# Set rpath
# use, i.e. don't skip the full RPATH for the build tree
SET(CMAKE_SKIP_BUILD_RPATH  FALSE)
# when building, don't use the install RPATH already
# (but later on when installing)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE) 
# the RPATH to be used when installing, but only if it's not a system directory
LIST(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)
IF("${isSystemDir}" STREQUAL "-1")
   SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
ENDIF("${isSystemDir}" STREQUAL "-1")
# add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# Documentation
add_subdirectory( documentation )

# Tests
if( TESTS )
	enable_testing()
	add_subdirectory( tests )
endif( TESTS )

# Examples (only with MPI)
if( NOT NOMPI )
	if( EXAMPLES )
		# Add examples
		add_subdirectory( examples )
	endif()
endif( NOT NOMPI )

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
	grid
	blocks
	io
	perf
	types )
if( NOMPI )
	set_target_properties( asagi
		PROPERTIES OUTPUT_NAME asagi_nompi )
else( NOMPI )
	target_link_libraries( asagi
	    mpi
	    ${MPI_C_LIBRARIES} )
endif( NOMPI )

##### Installation #####
install( TARGETS asagi
	DESTINATION lib )
install( FILES include/asagi.h include/asagi.inc
	DESTINATION include )


##### Packaging #####
INCLUDE(InstallRequiredSystemLibraries)

SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${CMAKE_PROJECT_NAME})
SET(CPACK_PACKAGE_VENDOR "Sebastian Rettenberger")
#SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/ReadMe.txt")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")
SET(CPACK_PACKAGE_VERSION_MAJOR ${ASAGI_VERSION_MAJOR})
SET(CPACK_PACKAGE_VERSION_MINOR ${ASAGI_VERSION_MINOR})
SET(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_SOURCE_IGNORE_FILES
	"\\\\.svn/"
	"\\\\.kdev4"
	"~$"
	"/preprocess/"
	"/test_progs/"
)

SET(CPACK_PACKAGE_INSTALL_DIRECTORY "CMake ${CMake_VERSION_MAJOR}.${CMake_VERSION_MINOR}")
INCLUDE(CPack)