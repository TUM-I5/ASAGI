##### Files #####
set( perfSources
	counter.cpp )

##### Library #####
add_library( perf
	${perfSources} )
if( NOT STATIC_LIB )
	set_target_properties( perf PROPERTIES
		COMPILE_FLAGS -fPIC )
endif()
