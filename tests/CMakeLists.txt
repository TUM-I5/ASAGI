##### Files #####
# Simple
set( SimpleSources
	simple.cpp )

##### Executables #####
# Simple
add_executable( simple
	${SimpleSources} )
target_link_libraries( simple
	database )