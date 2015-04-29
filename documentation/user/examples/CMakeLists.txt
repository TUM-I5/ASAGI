##### Files #####
# C
set( MinCSources
    minimal.c )
    
# C++
set( MinCppSources
    minimal.cpp )

# Fortran
set( MinF90Sources
	minimal.f90
	${CMAKE_SOURCE_DIR}/include/asagi.f90 )

# Add MPI header files to the include path
include_directories( SYSTEM ${MPI_Fortran_INCLUDE_PATH} )

##### Executables #####
# C
add_executable( c_minimal
    ${MinCSources} )
target_link_libraries( c_minimal
	${asagiTarget} ${asagiLibs}
	${MPI_C_LIBRARIES} ${MPI_CXX_LIBRARIES} )

# C++
add_executable( cpp_minimal
    ${MinCppSources} )
target_link_libraries( cpp_minimal
	${asagiTarget} ${asagiLibs}
	${MPI_C_LIBRARIES} ${MPI_CXX_LIBRARIES} )

# Fortran
add_executable( f90_minimal
	${MinF90Sources} )
target_link_libraries( f90_minimal
	${asagiTarget} ${asagiLibs}
	${MPI_C_LIBRARIES} ${MPI_CXX_LIBRARIES} )
set_target_properties( f90_minimal PROPERTIES
	LINKER_LANGUAGE Fortran )
