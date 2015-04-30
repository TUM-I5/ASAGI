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

# Tests in subdirectories want to include globaltest.h
include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )

##### Find CxxTest #####
# The current CMake find module only supports cxxtest < 4.0
set( CMAKE_INCLUDE_PATH
	${CMAKE_INCLUDE_PATH}
	${CMAKE_SOURCE_DIR}/tools/cxxtest )
set( CXXTEST_TESTGEN_ARGS --error-printer --template=${CMAKE_CURRENT_SOURCE_DIR}/run.tpl )
find_package( CxxTest )

# Simplify add_test_macro
function( unittest_add_test name header )
	get_filename_component( basename ${header} NAME_WE )

	cxxtest_add_test( ${name} ${basename}.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/${header} )
	target_link_libraries( ${name} ${asagiTarget} ${asagiLibs} )
	
	# Add grid dependencies?
	if( ARGV2 )
	    set_tests_properties( ${name} PROPERTIES DEPENDS prepare )
	endif( ARGV2 )
endfunction( unittest_add_test )

if( CXXTEST_FOUND )
	include_directories( ${CXXTEST_INCLUDE_DIR} )

	# Tests
	add_subdirectory( cache )
	add_subdirectory( grid )
	add_subdirectory( io )
	if( NOT NONUMA )
	    add_subdirectory( numa )
	    add_subdirectory( threads )
	endif( NOT NONUMA )
	add_subdirectory( types )
endif( CXXTEST_FOUND )