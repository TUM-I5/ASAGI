##### Files #####
set( gridSources
    adaptivegridcontainer.cpp
	grid.cpp
    gridcontainer.cpp
    localcachegrid.cpp
	multigrid.cpp
	simplegridcontainer.cpp
	staticgrid.cpp )
if( NOT NOMPI )
	set( gridSources
		${gridSources}
		largegrid.cpp
		mpicachegrid.cpp )
endif( NOT NOMPI )

##### Library #####
add_library( grid
	${gridSources} )
if( NOT STATIC_LIB )
	set_target_properties( grid PROPERTIES
		COMPILE_FLAGS -fPIC )
endif()
