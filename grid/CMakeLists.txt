##### Files #####
set( gridSources
    container.cpp
	grid.cpp )

##### Add subdirectory #####
add_subdirectory( level )

##### Add to library #####
add_sources( ${gridSources} )