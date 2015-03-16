##### Files #####
set( gridSources
    container.cpp
#    adaptivegridcontainer.cpp
	grid.cpp
#    gridcontainer.cpp
#    localcachegrid.cpp
#	localstaticgrid.cpp
#	multigrid.cpp
#	passthrough.cpp
#	simplecontainer.cpp
 )
#if( NOT NOMPI )
#	set( gridSources
#		${gridSources}
#		distcachegrid.cpp
#		diststaticgrid.cpp )
#endif( NOT NOMPI )

##### Add subdirectory #####
add_subdirectory( level )

##### Add to library #####
add_sources( ${gridSources} )