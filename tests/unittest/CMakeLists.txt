# Tests in subdirectories want to include globaltest.h
include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )

##### Find CxxTest #####
# The current CMake find module only supports cxxtest < 4.0
set( CMAKE_INCLUDE_PATH
	${CMAKE_INCLUDE_PATH}
	${CMAKE_SOURCE_DIR}/tools/cxxtest )
set( CXXTEST_TESTGEN_ARGS --error-printer --template=${CMAKE_CURRENT_SOURCE_DIR}/run.tpl )
find_package( CxxTest )

# Simplify add_test_macro
function( unittest_add_test name header )
	get_filename_component( basename ${header} NAME_WE )

	cxxtest_add_test( ${name} ${basename}.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/${header} )
	target_link_libraries( ${name} asagi )
endfunction( unittest_add_test )

if( CXXTEST_FOUND )
	include_directories( ${CXXTEST_INCLUDE_DIR} )

	# Tests
	add_subdirectory( blocks )
	add_subdirectory( grid )
	add_subdirectory( types )
	add_subdirectory( io )
endif( CXXTEST_FOUND )