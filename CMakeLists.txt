project( database )

# Version
cmake_minimum_required( VERSION 2.8 )

##### Files #####
# Lib Sources
set( LibSources
	database.cpp
	database_f77.cpp
	database_f95.cpp
	grid.cpp )

##### Other configuration #####
# Additional cmake modules
set( CMAKE_MODULE_PATH
	${CMAKE_MODULE_PATH}
	"${CMAKE_SOURCE_DIR}/CMake/Modules/" )

# Enable warnings
add_definitions( -Wall )

# Find png
find_package( PNG )
if( PNG_FOUND )
	add_definitions( -DWITH_PNG )
endif()

# Add static libaries
add_subdirectory( fortran )
add_subdirectory( io )

# Add library include files for all exectuables
include_directories( ${CMAKE_SOURCE_DIR}/include )

# Add tests
add_subdirectory( tests )

##### Excecutables #####
# Library
add_library( database SHARED
	${LibSources} )
target_link_libraries( database
	fortran
	io
	netcdf )