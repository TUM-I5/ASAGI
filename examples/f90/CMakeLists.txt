##### Files #####
# Simple
set( SimpleF90Sources
	simple.f90 )

# Buf
set( BufF90Sources
	buf.f90 )

# Add MPI header files to the include path
include_directories( ${MPI_Fortran_INCLUDE_PATH} SYSTEM )

##### Executables #####
# Simple
add_executable( f90_simple
	${SimpleF90Sources} )
target_link_libraries( f90_simple
	asagi
	${MPI_Fortran_LIBRARIES} )
set_target_properties( f90_simple PROPERTIES
	LINKER_LANGUAGE Fortran
	OUTPUT_NAME simple )

# Buf
add_executable( f90_buf
	${BufF90Sources} )
target_link_libraries( f90_buf
	asagi
	${MPI_Fortran_LIBRARIES} )
set_target_properties( f90_buf PROPERTIES
	LINKER_LANGUAGE Fortran
	OUTPUT_NAME buf )
