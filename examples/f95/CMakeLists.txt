##### Files #####
# Simple
set( SimpleF95Sources
	simple.f95 )

# Buf
set( BufF95Sources
	buf.f95 )

##### Executables #####
# Simple
add_executable( f95_simple
	${SimpleF95Sources} )
target_link_libraries( f95_simple
	asagi )
set_target_properties( f95_simple PROPERTIES
	OUTPUT_NAME simple )

# Buf
add_executable( f95_buf
	${BufF95Sources} )
target_link_libraries( f95_buf
	asagi )
set_target_properties( f95_buf PROPERTIES
	OUTPUT_NAME buf )