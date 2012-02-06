project( asagi )

# Version
cmake_minimum_required( VERSION 2.8 )

##### Files #####
# Lib Sources
set( LibSources
	asagi.cpp
	asagi_f95.cpp
	grid.cpp
	simplegrid.cpp )

##### User configuration #####
option( STATIC_LIB "Compile asagi as static library" OFF )
option( FORTRAN_SUPPORT "Enable Fortran support" ON )
option( EXAMPLES "Compile examples" OFF )

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
# Call this before searching for mpi
enable_language( Fortran )

# Find MPI
find_package( MPI REQUIRED )
include_directories( ${MPI_C_INCLUDE_PATH} SYSTEM )

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
add_subdirectory( fortran )
add_subdirectory( io )
add_subdirectory( types )

# Doxygen
find_package( Doxygen )
if( DOXYGEN_FOUND )
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
		${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
	add_custom_target( doc
		${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		COMMENT "Generating API documentation with Doxygen" VERBATIM )
endif( DOXYGEN_FOUND )

# Add tests
enable_testing()
#add_subdirectory( tests )

if( EXAMPLES )
	# Add examples
	add_subdirectory( examples )
endif()

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
	comm
	io
	types
	${MPI_C_LIBARARIES} ${MPI_CXX_LIBRARIES} )