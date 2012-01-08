project( asagi )

# Version
cmake_minimum_required( VERSION 2.8 )

##### Files #####
# Lib Sources
set( LibSources
	arraygrid.cpp
	asagi.cpp
	asagi_f95.cpp
	basicgrid.cpp
	grid.cpp )

##### Other configuration #####
# Additional cmake modules
set( CMAKE_MODULE_PATH
	${CMAKE_MODULE_PATH}
	${CMAKE_SOURCE_DIR}/CMake/Modules/ )

# Enable warnings
add_definitions( -Wall )

# Enable fortran
# Call this before searching for mpi
enable_language( Fortran )

# Find MPI
find_package( MPI REQUIRED )
include_directories( ${MPI_C_INCLUDE_PATH} SYSTEM )

# Find png
find_package( PNG )
if( PNG_FOUND )
	add_definitions( -DWITH_PNG )
endif()

# Add library include files for all exectuables
include_directories( ${CMAKE_SOURCE_DIR}/include )

# Add static libaries
add_subdirectory( fortran )
add_subdirectory( io )

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
add_subdirectory( tests )

# Add examples
add_subdirectory( examples )

##### Excecutables #####
# Library
add_library( asagi SHARED
	${LibSources} )
target_link_libraries( asagi
	io
	${MPI_C_LIBARARIES} ${MPI_CXX_LIBRARIES} )