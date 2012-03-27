##### Find Latex #####
find_package( LATEX )

##### Find Doxygen #####
find_package( Doxygen )

##### Input files
set( DocMain documentation.tex )
set( DocInputs
	doxygen.sty
	installation.tex
	usage.tex
	interface.tex
	linux-cluster.tex
	troubleshooting.tex
	minimal.c
	minimal.cpp
	minimal.f90
	images/cell-centered.tex
	images/vertex-centered.tex )
set( DocInterfaceInputs
	interfacedoc/group__c__interface.tex
	interfacedoc/group__cxx__interface.tex
	interfacedoc/group__f__interface.tex )

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

	if( DOXYGEN_FOUND )
		# Also depend on doxygen latex files if doxygen is available
		set( DocGenerated ${DocInterfaceInputs} )
	endif( DOXYGEN_FOUND )
	add_latex_document( ${DocMain}
		INPUTS ${DocInputs}
		IMAGE_DIRS images
		DEPENDS ${DocGenerated}
		DEFAULT_SAFEPDF
		MANGLE_TARGET_NAMES )
	add_custom_target( doc DEPENDS documentation_pdf )
	add_dependencies( documentation_pdf interfacedoc )
endif( PDFLATEX_COMPILER )

# Doxygen (API Doc)
if( DOXYGEN_FOUND )
	configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
		${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY )
	add_custom_target( apidoc
		${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		COMMENT "Generating API documentation with Doxygen" VERBATIM )

	# Generate Interface documentation with doxygen
	configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.interface.in
		${CMAKE_CURRENT_BINARY_DIR}/Doxyfile.interface @ONLY )
	add_custom_command( OUTPUT ${DocInterfaceInputs}
		COMMAND ${DOXYGEN_EXECUTABLE}
		ARGS ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile.interface
		DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile.interface ${InterfaceHeaders}
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		COMMENT "Generating Interface documentation with Doxygen" VERBATIM )
endif( DOXYGEN_FOUND )