##### Files #####
# Simple
set( SimpleCppSources
	simple.cpp )

##### Executables #####
# Simple
add_executable( cpp_simple
	${SimpleCppSources} )
target_link_libraries( cpp_simple
	database )
set_target_properties( cpp_simple PROPERTIES
	OUTPUT_NAME simple )