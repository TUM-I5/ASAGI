##### Files #####
set( commSources
	blockmanager.cpp
	lru.cpp )

##### Library #####
add_library( comm
	${commSources} )
if( NOT STATIC_LIB )
	set_target_properties( comm PROPERTIES
		COMPILE_FLAGS -fPIC )
endif()
