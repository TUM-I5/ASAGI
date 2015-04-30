# @file
#  This file is part of ASAGI.
# 
#  ASAGI is free software: you can redistribute it and/or modify
#  it under the terms of the GNU Lesser General Public License as
#  published by the Free Software Foundation, either version 3 of
#  the License, or  (at your option) any later version.
#
#  ASAGI is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with ASAGI.  If not, see <http://www.gnu.org/licenses/>.
#
#  Diese Datei ist Teil von ASAGI.
#
#  ASAGI ist Freie Software: Sie koennen es unter den Bedingungen
#  der GNU Lesser General Public License, wie von der Free Software
#  Foundation, Version 3 der Lizenz oder (nach Ihrer Option) jeder
#  spaeteren veroeffentlichten Version, weiterverbreiten und/oder
#  modifizieren.
#
#  ASAGI wird in der Hoffnung, dass es nuetzlich sein wird, aber
#  OHNE JEDE GEWAEHELEISTUNG, bereitgestellt; sogar ohne die implizite
#  Gewaehrleistung der MARKTFAEHIGKEIT oder EIGNUNG FUER EINEN BESTIMMTEN
#  ZWECK. Siehe die GNU Lesser General Public License fuer weitere Details.
#
#  Sie sollten eine Kopie der GNU Lesser General Public License zusammen
#  mit diesem Programm erhalten haben. Wenn nicht, siehe
#  <http://www.gnu.org/licenses/>.
# 
# @copyright 2015 Sebastian Rettenberger <rettenbs@in.tum.de>

# Tests may want to include more header files
include_directories( ${CMAKE_SOURCE_DIR} )

# Add test executable
function( add_test_executable name )
	if( ARGV1 AND NOMPI )
		# MPI test without MPI?
		return()
	endif( ARGV1 AND NOMPI )
	if( ARGV2 AND NOT FORTRAN_SUPPORT )
	    # Fortran test without Fortran?
	    return()
	endif( ARGV2 AND NOT FORTRAN_SUPPORT )
	
	if( ARGV2 )
	    set( sourceFiles
	        ${name}.f90 
	        ${CMAKE_SOURCE_DIR}/include/asagi.f90 )
	else( ARGV2 )
	    set( sourceFiles
	        ${name}.cpp )
	endif( ARGV2 )

	# Create the executable
	
	
	add_executable( test_${name} ${sourceFiles} )
	
	# Link with asagi
	target_link_libraries( test_${name} ${asagiTarget} ${asagiLibs} )
	if( ARGV1 )
		# Link with mpi library
		target_link_libraries( test_${name}
		    ${MPI_C_LIBRARIES} ${MPI_CXX_LIBRARIES} )
	endif( ARGV1 )
	
	# Fortran linker?
	if( ARGV2 )
        set_target_properties( test_${name} PROPERTIES
	        LINKER_LANGUAGE Fortran )
	endif( ARGV2 )
endfunction( add_test_executable )

# Adds an MPI test
function( add_test_run name command )
    if( ARGV2 )
        # MPI run
        if ( NOT NOMPI )
	        add_test( NAME ${name} COMMAND ${MPIEXEC} ${MPIEXEC_PREFLAGS}
		        ${MPIEXEC_NUMPROC_FLAG} ${ARGV2} "./${command}"
		        ${MPIEXEC_POSTFLAGS} )
		endif( NOT NOMPI )
	else( ARGV2 )
	    add_test( NAME ${name} COMMAND ${command} )
	endif( ARGV2 )
	
	# Depend on the prepare test
	set_tests_properties( ${name} PROPERTIES DEPENDS prepare )
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

# Test that generates the grid files
add_test( NAME prepare COMMAND test_prepare )

# Add directory for test header file
include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )

# Create test executables
#add_test_executable( grid1d YES )
#add_test_executable( grid1dpseudo YES )
#add_test_executable( grid2d YES )
#add_test_executable( grid3d YES )
#add_test_executable( largegrid1d YES )
#add_test_executable( adaptivegrid2d YES )
add_test_executable( fullgrid2d )
add_test_executable( fullnumagrid2d )
add_test_executable( fullmpigrid2d YES )
add_test_executable( fullmpinumagrid2d YES )
add_test_executable( fullmpinumacachegrid2d YES )

add_test_executable( cachegrid2d )
add_test_executable( cachenumagrid2d )
add_test_executable( cachempigrid2d YES )
add_test_executable( cachempinumagrid2d YES )

add_test_executable( passthroughgrid2d )

add_test_executable( fullgrid3d )
add_test_executable( fullgrid2dbs )
add_test_executable( fullgrid2dscale )
add_test_executable( fullmpinumagrid2d2 YES )

add_test_executable( fortran2d YES YES )

# Unit Tests
add_subdirectory( unittest )

# Tests
#add_test_run( Grid1D test_grid1d 2 )
#add_test_run( Grid1DPseudo test_grid1dpseudo 2 )
#add_test_run( Grid2D test_grid2d 2 )
#add_test_run( Grid3D test_grid3d 2 )
#add_test_run( LargeGrid1D test_largegrid1d 2 )
#add_test_run( AdaptiveGrid2D test_adaptivegrid2d 2 )
add_test_run( FullGrid2D test_fullgrid2d )
add_test_run( FullNumaGrid2D test_fullnumagrid2d )
add_test_run( FullMPIGrid2D test_fullmpigrid2d 2 )
add_test_run( FullMPINumaGrid2D test_fullmpinumagrid2d 2 )
add_test_run( FullMPINumaCacheGrid2D test_fullmpinumacachegrid2d 2 )

add_test_run( CacheGrid2D test_cachegrid2d )
add_test_run( CacheNumaGrid2D test_cachenumagrid2d )
add_test_run( CacheMPIGrid2D test_cachempigrid2d 2 )
add_test_run( CacheMPINumaGrid2D test_cachempinumagrid2d 2 )

add_test_run( PassThroughGrid2D test_passthroughgrid2d )

add_test_run( FullGrid3D test_fullgrid3d )
add_test_run( FullGrid2DBlockSize test_fullgrid2dbs )
add_test_run( FullGrid2DScaled test_fullgrid2dscale )
add_test_run( FullMPINumaGrid2D2 test_fullmpinumagrid2d2 2 )

add_test_run( Fortran2D test_fortran2d 2 )
