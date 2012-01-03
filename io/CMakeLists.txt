##### Files #####
set( ioSources
	netcdf.cpp )

##### Settings #####
# Find netcdf
set( NETCDF_CXX "YES" )
find_package( NetCDF REQUIRED )

##### Library #####
add_library( io
	${ioSources} )
target_link_libraries( io
	${NETCDF_LIBRARIES} )
set_target_properties( io PROPERTIES
	COMPILE_FLAGS -fPIC )
