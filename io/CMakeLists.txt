##### Files #####
set( ioSources
	netcdf.cpp
	png.cpp )

if( PNG_FOUND )
	set( ioSources
	${ioSources} png.cpp )
endif()


##### Settings #####
# Find netcdf
set( NETCDF_CXX4 "YES" )
find_package( NetCDF REQUIRED )

##### Library #####
add_library( io
	${ioSources} )
target_link_libraries( io
	${NETCDF_LIBRARIES}
	${PNG_LIBRARY} )
set_target_properties( io PROPERTIES
	COMPILE_FLAGS -fPIC )
