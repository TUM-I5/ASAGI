##### Files #####
set( ioSources
	netcdf.cpp
	png.cpp )

##### Settings #####
# Find netcdf
set( NETCDF_CXX "YES" )
find_package( NetCDF REQUIRED )

# Find png
find_package( PNG REQUIRED )

##### Library #####
add_library( io
	${ioSources} )
target_link_libraries( io
	${NETCDF_LIBRARIES}
	${PNG_LIBRARY} )
set_target_properties( io PROPERTIES
	COMPILE_FLAGS -fPIC )
