##### Find Doxygen #####
find_package( Doxygen )

if( DOXYGEN_FOUND )
	# Find Latex
	find_package( LATEX )

	# API Doc
	configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
		${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY )
	add_custom_target( apidoc
		${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		COMMENT "Generating API documentation with Doxygen" VERBATIM )

	# Generate Interface documentation with doxygen
	configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.interface.in
		${CMAKE_CURRENT_BINARY_DIR}/Doxyfile.interface @ONLY )
	add_custom_target( doc
		${DOXYGEN_EXECUTABLE}
		${CMAKE_CURRENT_BINARY_DIR}/Doxyfile.interface
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		COMMENT "Generating User Manual with Doxygen and Latex" VERBATIM )
	if ( PDFLATEX_COMPILER )
		# Compiling user manual makes only sence is latex is installed
		# This will also not work if make is not available
		add_custom_command( TARGET doc
			POST_BUILD
			COMMAND make
			COMMAND ${CMAKE_COMMAND} -E rename
				${CMAKE_CURRENT_BINARY_DIR}/doc/refman.pdf
				${CMAKE_CURRENT_BINARY_DIR}/doc.pdf
			WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc )
	endif( PDFLATEX_COMPILER )
endif( DOXYGEN_FOUND )