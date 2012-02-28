# Tests may want to include more header files
include_directories( ${CMAKE_SOURCE_DIR} )

##### Prepare tests #####
# Files
set( TestPrepareSources
	prepare.cpp )

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

# Create test executables
add_executable( test_grid1d grid1d.cpp )
target_link_libraries( test_grid1d asagi ${MPI_C_LIBRARIES} )
add_dependencies( test_grid1d test_prepare )

add_executable( test_grid1dpseudo grid1dpseudo.cpp )
target_link_libraries( test_grid1dpseudo asagi ${MPI_C_LIBRARIES} )
add_dependencies( test_grid1dpseudo test_prepare )

add_executable( test_grid2d grid2d.cpp )
target_link_libraries( test_grid2d asagi ${MPI_C_LIBRARIES} )
add_dependencies( test_grid2d test_prepare )

add_executable( test_grid3d grid3d.cpp )
target_link_libraries( test_grid3d asagi ${MPI_C_LIBRARIES} )
add_dependencies( test_grid3d test_prepare )

# Tests
add_test( NAME Grid1D COMMAND test_grid1d )
add_test( NAME Grid1DPseudo COMMAND test_grid1dpseudo )
add_test( NAME Grid2D COMMAND test_grid2d )
add_test( NAME Grid3D COMMAND test_grid3d )

# Performance tests
# if ( CMAKE_BUILD_TYPE STREQUAL "Release" )
# 	add_subdirectory( performance )
# else()
# 	message( WARNING "Performance tests are only enable in release build!" )
# endif()