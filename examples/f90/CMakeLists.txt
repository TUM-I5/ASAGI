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

##### Files #####
# Simple
set( SimpleF90Sources
	simple.f90
	${CMAKE_SOURCE_DIR}/include/asagi.f90 )

# Buf
set( BufF90Sources
	buf.f90
	${CMAKE_SOURCE_DIR}/include/asagi.f90 )

# Add MPI header files to the include path
include_directories( SYSTEM ${MPI_Fortran_INCLUDE_PATH} )

##### Executables #####
# Simple
add_executable( f90_simple
	${SimpleF90Sources} )
target_link_libraries( f90_simple
	${asagiTarget} ${asagiLibs}
	${MPI_C_LIBRARIES} ${MPI_CXX_LIBRARIES} )
set_target_properties( f90_simple PROPERTIES
	LINKER_LANGUAGE Fortran
	OUTPUT_NAME simple )

# Buf
add_executable( f90_buf
	${BufF90Sources} )
target_link_libraries( f90_buf
	${asagiTarget} ${asagiLibs}
	${MPI_C_LIBRARIES} ${MPI_CXX_LIBRARIES} )
set_target_properties( f90_buf PROPERTIES
	LINKER_LANGUAGE Fortran
	OUTPUT_NAME buf )
