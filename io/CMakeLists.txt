##### Files #####
set( ioSources
	netcdf.cpp )

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
if( NOT STATIC_LIB )
	set_target_properties( io PROPERTIES
		COMPILE_FLAGS -fPIC )
endif()
