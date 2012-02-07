##### Files #####
# Simple
set( SimpleF95Sources
	simple.f95 )

# Buf
set( BufF95Sources
	buf.f95 )

# Add MPI header files to the include path
include_directories( ${MPI_Fortran_INCLUDE_PATH} SYSTEM )

##### Executables #####
# Simple
add_executable( f95_simple
	${SimpleF95Sources} )
target_link_libraries( f95_simple
	asagi
	${MPI_Fortran_LIBRARIES} )
set_target_properties( f95_simple PROPERTIES
	OUTPUT_NAME simple )

# Buf
add_executable( f95_buf
	${BufF95Sources} )
target_link_libraries( f95_buf
	asagi
	${MPI_Fortran_LIBRARIES} )
set_target_properties( f95_buf PROPERTIES
	OUTPUT_NAME buf )