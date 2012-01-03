##### Files #####
# Simple
set( SimpleSources
	simple.c )

##### Executables #####
# Simple
add_executable( simple
	${SimpleSources} )
target_link_libraries( simple
	database )