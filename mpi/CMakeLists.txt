##### Files #####
set( syncSources
	mutex.cpp )

##### Library #####
add_library( sync
	${syncSources} )
if( NOT STATIC_LIB )
	set_target_properties( sync PROPERTIES
		COMPILE_FLAGS -fPIC )
endif()
