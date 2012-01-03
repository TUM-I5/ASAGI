##### Files #####
# Simple
set( SimpleF95Sources
	simple.f95 )
#	../../database.f95 )

##### Build fortran #####
enable_language( Fortran )

include_directories( ${CMAKE_SOURCE_DIR} )

##### Executables #####
# Simple
add_executable( f95_simple
	${SimpleF95Sources} )
target_link_libraries( f95_simple
	database )
set_target_properties( f95_simple PROPERTIES
	OUTPUT_NAME simple )