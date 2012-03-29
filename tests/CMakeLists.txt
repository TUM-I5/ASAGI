# Tests may want to include more header files
include_directories( ${CMAKE_SOURCE_DIR} )

# Adds an MPI test
function( add_mpi_test name command numproc )
	add_test( NAME ${name} COMMAND ${MPIEXEC} ${MPIEXEC_PREFLAGS}
		${MPIEXEC_NUMPROC_FLAG} ${numproc} ${command}
		${MPIEXEC_POSTFLAGS} )
endfunction( add_mpi_test )

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

add_executable( test_largegrid1d largegrid1d.cpp )
target_link_libraries( test_largegrid1d asagi ${MPI_C_LIBRARIES} )
add_dependencies( test_largegrid1d test_prepare )

# Unit Tests
add_subdirectory( unittest )

# Tests
add_mpi_test( Grid1D test_grid1d 2 )
add_mpi_test( Grid1DPseudo test_grid1dpseudo 2 )
add_mpi_test( Grid2D test_grid2d 2 )
add_mpi_test( Grid3D test_grid3d 2 )

add_mpi_test( LargeGrid1D test_largegrid1d 2 )

# Performance tests
# if ( CMAKE_BUILD_TYPE STREQUAL "Release" )
# 	add_subdirectory( performance )
# else()
# 	message( WARNING "Performance tests are only enable in release build!" )
# endif()