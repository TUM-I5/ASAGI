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