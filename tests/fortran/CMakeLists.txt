##### Files #####
# Simple
set( SimpleFSources
	simple.f )

##### Build fortran #####
enable_language( Fortran )

include_directories( ${CMAKE_SOURCE_DIR} )

##### Executables #####
# Simple
add_executable( f_simple
	${SimpleFSources} )
target_link_libraries( f_simple
	database )
set_target_properties( f_simple PROPERTIES
	OUTPUT_NAME simple
	LINKER_LANGUAGE Fortran )