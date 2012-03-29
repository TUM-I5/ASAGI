##### Files #####
set( blocksSources
	blockmanager.cpp
	lru.cpp )

##### Library #####
add_library( blocks
	${blocksSources} )
if( NOT STATIC_LIB )
	set_target_properties( blocks PROPERTIES
		COMPILE_FLAGS -fPIC )
endif()
