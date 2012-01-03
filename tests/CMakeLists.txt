# Tests may want to include more header files
include_directories( ${CMAKE_SOURCE_DIR} )

##### Prepare tests #####
# Files
set( TestPrepareSources
	prepare.cpp )

# Find netcdf
set( NETCDF_CXX4 "YES" )
find_package( NetCDF REQUIRED )

# Add target
add_executable( test_prepare
	${TestPrepareSources} )
target_link_libraries( test_prepare
	${NETCDF_LIBRARIES} )

# Always execute test_prepare after build
add_custom_command( TARGET test_prepare
	POST_BUILD
	COMMAND test_prepare )

# Add directory for generated header files
include_directories( ${CMAKE_CURRENT_BINARY_DIR} )

# Performance tests
if ( CMAKE_BUILD_TYPE STREQUAL "Release" )
	add_subdirectory( performance )
else()
	message( WARNING "Performance tests are only enable in release build!" )
endif()