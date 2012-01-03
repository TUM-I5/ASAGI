##### Files #####
# Simple
set( SimpleCppSources
	simple.cpp )

# ExportPng
set( ExportPngCppSources
	exportpng.cpp )

##### Executables #####
# Simple
add_executable( cpp_simple
	${SimpleCppSources} )
target_link_libraries( cpp_simple
	database )
set_target_properties( cpp_simple PROPERTIES
	OUTPUT_NAME simple )

# ExportPng
add_executable( cpp_exportpng
	${ExportPngCppSources} )
target_link_libraries( cpp_exportpng
	database )
set_target_properties( cpp_exportpng PROPERTIES
	OUTPUT_NAME exportpng )