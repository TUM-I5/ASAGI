##### Files #####
set( typesSources
	basictype.cpp
	type.cpp )

##### Library #####
add_library( types
	${typesSources} )
set_target_properties( types PROPERTIES
	COMPILE_FLAGS -fPIC )
