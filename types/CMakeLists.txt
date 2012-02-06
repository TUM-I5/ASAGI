##### Files #####
set( typesSources
	basictype.cpp
	type.cpp )

##### Library #####
add_library( types
	${typesSources} )
if( NOT STATIC_LIB )
	set_target_properties( types PROPERTIES
		COMPILE_FLAGS -fPIC )
endif()
