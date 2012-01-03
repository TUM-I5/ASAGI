##### Files #####
set( fortranSources
	string.cpp )

##### Settings #####

##### Library #####
add_library( fortran
	${fortranSources} )
target_link_libraries( fortran
	${NETCDF_LIBRARIES} )
set_target_properties( fortran PROPERTIES
	COMPILE_FLAGS -fPIC )
