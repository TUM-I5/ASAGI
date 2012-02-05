##### Files #####
set( ioSources
	netcdf.cpp
	png.cpp )

if( PNG_FOUND )
	set( ioSources
	${ioSources} png.cpp )
endif()

##### Library #####
add_library( io
	${ioSources} )
target_link_libraries( io
	${NETCDF_LIBRARIES}
	${PNG_LIBRARY} )
set_target_properties( io PROPERTIES
	COMPILE_FLAGS -fPIC )
