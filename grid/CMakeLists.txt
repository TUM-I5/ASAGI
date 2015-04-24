##### Files #####
set( gridSources
	grid.cpp )

##### Add subdirectory #####
add_subdirectory( level )

##### Add to library #####
add_sources( ${gridSources} )