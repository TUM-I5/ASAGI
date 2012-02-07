##### Files #####
# Simple
set( SimpleCppSources
	simple.cpp )

# Array
set( ArrayCppSources
	array.cpp )

# ExportPng
set( ExportPngCppSources
	exportpng.cpp )

##### Executables #####
# Simple
add_executable( cpp_simple
	${SimpleCppSources} )
target_link_libraries( cpp_simple
	asagi
	${MPI_C_LIBRARIES} )
set_target_properties( cpp_simple PROPERTIES
	OUTPUT_NAME simple )

# Array
add_executable( cpp_array
	${ArrayCppSources} )
target_link_libraries( cpp_array
	asagi
	${MPI_C_LIBRARIES} )
set_target_properties( cpp_array PROPERTIES
	OUTPUT_NAME array )

if( PNG_FOUND )
	# ExportPng
	add_executable( cpp_exportpng
		${ExportPngCppSources} )
	target_link_libraries( cpp_exportpng
		asagi
		${MPI_C_LIBRARIES} )
	set_target_properties( cpp_exportpng PROPERTIES
		OUTPUT_NAME exportpng )
endif()