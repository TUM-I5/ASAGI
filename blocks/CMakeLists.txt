##### Files #####
set( commSources
	blockmanager.cpp
	lru.cpp )

##### Library #####
add_library( comm
	${commSources} )
set_target_properties( comm PROPERTIES
	COMPILE_FLAGS -fPIC )
