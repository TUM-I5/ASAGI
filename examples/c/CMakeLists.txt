##### Files #####
# Simple
set( SimpleCSources
	simple.c )

##### Executables #####
# Simple
add_executable( c_simple
	${SimpleCSources} )
target_link_libraries( c_simple
	${asagiTarget} ${asagiLibs}
	${MPI_C_LIBRARIES} ${MPI_CXX_LIBRARIES} ) # Link against CXX libs
	                                          # since asagi is compiled with mpicxx
set_target_properties( c_simple PROPERTIES
	LINKER_LANGUAGE C
	OUTPUT_NAME simple )