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
# @copyright 2015-2016 Sebastian Rettenberger <rettenbs@in.tum.de>

# Tests may want to include more header files
include_directories( ${CMAKE_SOURCE_DIR} )

# Add test executable
function( add_test_executable name )
    if( ARGV1 AND NONUMA )
        # NUMA test without NUMA?
        return()
    endif( ARGV1 AND NONUMA )
	if( ARGV2 AND NOMPI )
		# MPI test without MPI?
		return()
	endif( ARGV2 AND NOMPI )
	if( ARGV3 AND NOT FORTRAN_SUPPORT )
	    # Fortran test without Fortran?
	    return()
	endif( ARGV3 AND NOT FORTRAN_SUPPORT )
	
	if( ARGV3 )
	    set( sourceFiles
	        ${name}.f90 
	        ${CMAKE_SOURCE_DIR}/include/asagi.f90 )
	else( ARGV3 )
	    set( sourceFiles
	        ${name}.cpp )
	endif( ARGV3 )

	# Create the executable
	
	
	add_executable( test_${name} ${sourceFiles} )
	
	# Link with asagi
	target_link_libraries( test_${name} ${asagiTarget} ${asagiLibs} )
	if( ARGV2 )
		# Link with mpi library
		target_link_libraries( test_${name}
		    ${MPI_C_LIBRARIES} ${MPI_CXX_LIBRARIES} )
	endif( ARGV2 )
	
	# Fortran linker?
	if( ARGV3 )
        set_target_properties( test_${name} PROPERTIES
	        LINKER_LANGUAGE Fortran )
	endif( ARGV3 )
endfunction( add_test_executable )

# Adds an MPI test
function( add_test_run name command )
    if( ARGV2 )
        # NUMA run
        if( NONUMA )
            return()
        endif( NONUMA )
    endif( ARGV2 )
    
    if( ARGV3 )
        # MPI run
        if ( NOMPI )
            return()
        endif( NOMPI )
        add_test( NAME ${name} COMMAND ${MPIEXEC} ${MPIEXEC_PREFLAGS}
	        ${MPIEXEC_NUMPROC_FLAG} ${ARGV3} "./${command}"
	        ${MPIEXEC_POSTFLAGS} )
	else( ARGV3 )
	    add_test( NAME ${name} COMMAND ${command} )
	endif( ARGV3 )
	
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
add_test_executable( fullgrid2d )
add_test_executable( fullnumagrid2d YES )
add_test_executable( fullnumacachegrid2d YES )
add_test_executable( fullmpiwingrid2d NO YES )
add_test_executable( fullmpinumagrid2d YES YES )
add_test_executable( fullmpiwinnumacachegrid2d YES YES )
add_test_executable( fullmpithreadgrid2d NO YES )
add_test_executable( fullmpithreadnumacachegrid2d YES YES )

add_test_executable( cachegrid2d )
add_test_executable( cachenumagrid2d YES )
add_test_executable( cachempiwingrid2d NO YES )
add_test_executable( cachempiwingrid2d2 NO YES )
add_test_executable( cachempiwinnumagrid2d YES YES )
add_test_executable( cachempithreadgrid2d NO YES )
add_test_executable( cachempithreadnumagrid2d YES YES )

add_test_executable( passthroughgrid2d )

add_test_executable( fullgrid3d )
add_test_executable( fullgrid2dbs )
add_test_executable( fullgrid2dscale )
add_test_executable( fullgrid2darray )
add_test_executable( fullgrid2dstruct )
add_test_executable( fullmpinumagrid2d2 YES YES )

add_test_executable( fortran2d NO YES YES )

# Unit Tests
add_subdirectory( unittest )

# Tests
#add_test_run( Grid1D test_grid1d 2 )
#add_test_run( Grid1DPseudo test_grid1dpseudo 2 )
#add_test_run( Grid2D test_grid2d 2 )
#add_test_run( Grid3D test_grid3d 2 )
#add_test_run( LargeGrid1D test_largegrid1d 2 )
add_test_run( FullGrid2D test_fullgrid2d )
add_test_run( FullNumaGrid2D test_fullnumagrid2d YES )
add_test_run( FullNumaCacheGrid2D test_fullnumacachegrid2d YES )
add_test_run( FullMPIWinGrid2D test_fullmpiwingrid2d NO 2 )
add_test_run( FullMPINumaGrid2D test_fullmpinumagrid2d YES 2 )
add_test_run( FullMPIWinNumaCacheGrid2D test_fullmpiwinnumacachegrid2d YES 2 )
if( NOT THREADSAFE_MPI )
    add_test_run( FullMPIThreadGrid2D test_fullmpithreadgrid2d NO 2 )
    add_test_run( FullMPIThreadNumaCacheGrid2D test_fullmpithreadnumacachegrid2d YES 2 )
endif( NOT THREADSAFE_MPI )

add_test_run( CacheGrid2D test_cachegrid2d )
add_test_run( CacheNumaGrid2D test_cachenumagrid2d YES )
if( MPI3 )
    add_test_run( CacheMPIWinGrid2D test_cachempiwingrid2d NO 2 )
    add_test_run( CacheMPIWinGrid2D2 test_cachempiwingrid2d2 NO 2 )
    add_test_run( CacheMPIWinNumaGrid2D test_cachempiwinnumagrid2d YES 2 )
endif( MPI3 )
if( NOT THREADSAFE_MPI )
    add_test_run( CacheMPIThreadGrid2D test_cachempithreadgrid2d NO 2 )
    add_test_run( CacheMPIThreadNumaGrid2D test_cachempithreadnumagrid2d YES 2 )
endif( NOT THREADSAFE_MPI )

add_test_run( PassThroughGrid2D test_passthroughgrid2d )

add_test_run( FullGrid3D test_fullgrid3d )
add_test_run( FullGrid2DBlockSize test_fullgrid2dbs )
add_test_run( FullGrid2DScaled test_fullgrid2dscale )
add_test_run( FullGrid2DArray test_fullgrid2darray )
add_test_run( FullGrid2DStruct test_fullgrid2dstruct )
add_test_run( FullMPINumaGrid2D2 test_fullmpinumagrid2d2 YES 2 )

add_test_run( Fortran2D test_fortran2d NO 2 )
