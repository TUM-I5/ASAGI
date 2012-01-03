##### Files #####
# Simple
set( SimpleCSources
	simple.c )

##### Executables #####
# Simple
add_executable( c_simple
	${SimpleCSources} )
target_link_libraries( c_simple
	asagi )
set_target_properties( c_simple PROPERTIES
	OUTPUT_NAME simple )