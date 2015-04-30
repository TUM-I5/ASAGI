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

##### Find Doxygen #####
find_package( Doxygen )

if( DOXYGEN_FOUND )
	# Find Latex
	find_package( LATEX )

	# API Doc
	configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
		${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY )
	add_custom_target( devdoc
		${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		COMMENT "Generating developer documentation with Doxygen" VERBATIM )

	# Generate Interface documentation with doxygen
	configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.interface.in
		${CMAKE_CURRENT_BINARY_DIR}/Doxyfile.interface @ONLY )
	add_custom_target( doc
		${DOXYGEN_EXECUTABLE}
		${CMAKE_CURRENT_BINARY_DIR}/Doxyfile.interface
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		COMMENT "Generating User Manual with Doxygen and Latex" VERBATIM )
	if ( PDFLATEX_COMPILER )
	    if( WIN32 OR MSVC )
          set (NULDEV "NUL")
        else( WIN32 OR MSVC )
          set (NULDEV "/dev/null")
        endif( WIN32 OR MSVC )
	
		# Compiling user manual makes only sence is latex is installed
		# This will also not work if make is not available
		add_custom_command( TARGET doc
			POST_BUILD
			COMMAND make > ${NULDEV}
			COMMAND ${CMAKE_COMMAND} -E rename
				${CMAKE_CURRENT_BINARY_DIR}/doc/refman.pdf
				${CMAKE_CURRENT_BINARY_DIR}/doc.pdf
			WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc )
	endif( PDFLATEX_COMPILER )
endif( DOXYGEN_FOUND )

##### Add subdirectory #####
add_subdirectory( user )