# Tests may want to include more header files
include_directories( ${CMAKE_SOURCE_DIR} )

# Add test executable
function( add_test_executable name )
	if( ARGV1 AND NOMPI )
		# MPI test without MPI?
		return()
	endif( ARGV1 AND NOMPI )

	# Create the executable
	add_executable( test_${name} ${name}.cpp )
	# Link with asagi
	target_link_libraries( test_${name} ${asagiTarget} ${asagiLibs} )
	if( ARGV1 )
		# Link with mpi library
		target_link_libraries( test_${name} ${MPI_C_LIBRARIES} )
	endif( ARGV1 )
	# Tests require tests.h (the generated header)
	set_source_files_properties( ${name}.cpp
	    PROPERTIES OBJECT_DEPENDS ${test_header} )
    add_dependencies( test_${name} generate_test_header )
endfunction( add_test_executable )

# Adds an MPI test
function( add_test_run name command )
    if( ARGV2 )
        # MPI run
        if ( NOT NOMPI )
	        add_test( NAME ${name} COMMAND ${MPIEXEC} ${MPIEXEC_PREFLAGS}
		        ${MPIEXEC_NUMPROC_FLAG} ${ARGV2} ${command}
		        ${MPIEXEC_POSTFLAGS} )
		endif( NOT NOMPI )
	else( ARGV2 )
	    add_test( NAME ${name} COMMAND ${command} )
	endif( ARGV2 )
endfunction( add_test_run )

##### Prepare tests #####
# Files
set( TestPrepareSources
	prepare.cpp )

# Add target
add_executable( test_prepare
	${TestPrepareSources} )
target_link_libraries( test_prepare
	${NETCDF_LIBRARIES} )

# Add rule to generate test header
add_custom_command( OUTPUT tests.h
	COMMAND test_prepare
	DEPENDS test_prepare )
add_custom_target( generate_test_header
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/tests.h )

# Define test header location
set( test_header ${CMAKE_CURRENT_BINARY_DIR}/tests.h )

# Add directory for generated header files
include_directories( ${CMAKE_CURRENT_BINARY_DIR} )

# Create test executables
add_test_executable( grid1d YES )
#add_test_executable( grid1dpseudo YES )
#add_test_executable( grid2d YES )
#add_test_executable( grid3d YES )
#add_test_executable( largegrid1d YES )
#add_test_executable( largegrid2d YES )
#add_test_executable( adaptivegrid2d YES )
add_test_executable( fullnumagrid2d )
add_test_executable( fullmpigrid2d YES )
add_test_executable( fullmpinumagrid2d YES )

add_test_executable( fullgrid2d )
add_test_executable( cachegrid2d )
add_test_executable( passthroughgrid2d )

# Unit Tests
add_subdirectory( unittest )

# Tests
#add_test_run( Grid1D test_grid1d 2 )
#add_test_run( Grid1DPseudo test_grid1dpseudo 2 )
#add_test_run( Grid2D test_grid2d 2 )
#add_test_run( Grid3D test_grid3d 2 )
#add_test_run( LargeGrid1D test_largegrid1d 2 )
#add_test_run( LargeGrid2D test_largegrid2d 2 )
#add_test_run( AdaptiveGrid2D test_adaptivegrid2d 2 )
add_test_run( FullNumaGrid2D test_fullnumagrid2d )
add_test_run( FullMPIGrid2D test_fullmpigrid2d 2 )
add_test_run( FullMPINumaGrid2D test_fullmpinumagrid2d 2 )

add_test_run( FullGrid2D test_fullgrid2d ) 
add_test_run( LocalCacheGrid2D test_cachegrid2d )
add_test_run( PassThroughGrid2D test_passthroughgrid2d )
