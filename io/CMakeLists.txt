##### Files #####
set( ioSources
	netcdfreader.cpp )

if( PNG_FOUND )
	set( ioSources
		${ioSources} pngwriter.cpp )
endif()

##### Add to library #####
add_sources( ${ioSources} )
