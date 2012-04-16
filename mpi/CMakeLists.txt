##### Files #####
set( syncSources
	mutex.cpp )

##### Library #####
add_library( mpi
	${syncSources} )
if( NOT STATIC_LIB )
	set_target_properties( mpi PROPERTIES
		COMPILE_FLAGS -fPIC )
endif()
