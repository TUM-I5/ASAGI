##### Files #####
# Simple
set( SimpleCSources
	simple.c )

##### Executables #####
# Simple
add_executable( c_simple
	${SimpleCSources} )
target_link_libraries( c_simple
	asagi
	${MPI_C_LIBRARIES} )
set_target_properties( c_simple PROPERTIES
	LINKER_LANGUAGE C
	OUTPUT_NAME simple )