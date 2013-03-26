##### Files #####
set( gridSources
    adaptivegridcontainer.cpp
	grid.cpp
    gridcontainer.cpp
    localcachegrid.cpp
	localstaticgrid.cpp
	multigrid.cpp
	passthroughgrid.cpp
	simplegridcontainer.cpp )
if( NOT NOMPI )
	set( gridSources
		${gridSources}
		distcachegrid.cpp
		diststaticgrid.cpp )
endif( NOT NOMPI )

##### Library #####
add_library( grid
	${gridSources} )
if( NOT STATIC_LIB )
	set_target_properties( grid PROPERTIES
		COMPILE_FLAGS -fPIC )
endif()
