##### Find Latex #####
find_package( LATEX )

##### Find Doxygen #####
find_package( Doxygen )

##### Input files
set( DocMain documentation.tex )
set( DocInputs
	installation.tex
	usage.tex
	linux-cluster.tex
	troubleshooting.tex
	minimal.c
	minimal.cpp
	minimal.f90
	images/cell-centered.tex
	images/vertex-centered.tex )

# User Manual
if( PDFLATEX_COMPILER )
	# Only generate latex output if latex was found
	include( UseLATEX )
	# Unset DVItoPS converter
	# When this variable is not set, latex -> ps -> pdf will not work
	# 1. We don't need it (doesn't work with hyperref anyway)
	# 2. Adding DEFAULT_SAFEPDF will remove the latex command form the "all"
	# target
	set( DVIPS_CONVERTER "DVIPS_CONVERTER-NOTFOUND" )

	add_latex_document( ${DocMain}
		INPUTS ${DocInputs}
		IMAGE_DIRS images
		DEFAULT_SAFEPDF
		MANGLE_TARGET_NAMES )
	add_custom_target( doc DEPENDS documentation_pdf )
endif( PDFLATEX_COMPILER )

# Doxygen (API Doc)
if( DOXYGEN_FOUND )
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
		${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
	add_custom_target( apidoc
		${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		COMMENT "Generating API documentation with Doxygen" VERBATIM )
endif( DOXYGEN_FOUND )