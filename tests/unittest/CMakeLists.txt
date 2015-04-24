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
	target_link_libraries( ${name} ${asagiTarget} ${asagiLibs} )
	
	# Add grid dependencies?
	if( ARGV2 )
	    set_tests_properties( ${name} PROPERTIES DEPENDS prepare )
	endif( ARGV2 )
endfunction( unittest_add_test )

if( CXXTEST_FOUND )
	include_directories( ${CXXTEST_INCLUDE_DIR} )

	# Tests
	add_subdirectory( cache )
	add_subdirectory( grid )
	add_subdirectory( io )
	add_subdirectory( numa )
	add_subdirectory( threads )
	add_subdirectory( types )
endif( CXXTEST_FOUND )