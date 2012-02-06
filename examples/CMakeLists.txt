# Add all test directories
add_subdirectory( c )
add_subdirectory( cpp )
if( FORTRAN_SUPPORT )
	add_subdirectory( f95 )
endif()