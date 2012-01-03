##### Prepare perf tests #####
# Files
set( PerfTestPrepareSources
	prepare.cpp )

# Build
add_executable( test_perf_prepare
	${PerfTestPrepareSources} )
# Build the global header file
add_dependencies( test_perf_prepare
	test_prepare )

# Prepare performance test files
add_custom_command( OUTPUT perftests.h
	COMMAND test_perf_prepare
	DEPENDS test_perf_prepare )

# Add directory for generated header files
include_directories( ${CMAKE_CURRENT_BINARY_DIR} )

##### Files #####
# Fastest possible access
set( TestPerfAccessFastSources
	access_fast.cpp
	perftests.h )

# Access with 2d array coords (no range check)
set( TestPerfAccess2dIntSources
	access_2d_int.cpp
	perftests.h )

# Access with real world coords but no range check (default value)
set( TestPerfAccess2dNoDefaultSources
	access_2d_nodefault.cpp
	perftests.h )

# Access with real world coords (with default value)
set( TestPerfAccess2dSources
	access_2d.cpp
	perftests.h )

# Access through interface
set( TestPerfAccessInterfaceSources
	access_interface.cpp
	perftests.h )

##### Build tests #####
add_executable( test_perf_access_fast
	${TestPerfAccessFastSources} )
target_link_libraries( test_perf_access_fast
	asagi )

add_executable( test_perf_access_2d_int
	${TestPerfAccess2dIntSources} )
target_link_libraries( test_perf_access_2d_int
	asagi )

add_executable( test_perf_access_2d_nodefault
	${TestPerfAccess2dNoDefaultSources} )
target_link_libraries( test_perf_access_2d_nodefault
	asagi )

add_executable( test_perf_access_2d
	${TestPerfAccess2dSources} )
target_link_libraries( test_perf_access_2d
	asagi )

add_executable( test_perf_access_interface
	${TestPerfAccessInterfaceSources} )
target_link_libraries( test_perf_access_interface
	asagi )

##### Run tests #####
add_test( NAME PerfAccessFast
	COMMAND test_perf_access_fast )
add_test( NAME PerfAccess2dInt
	COMMAND test_perf_access_2d_int )
add_test( NAME PerfAccess2dNoDefault
	COMMAND test_perf_access_2d_nodefault )
add_test( NAME PerfAccess2d
	COMMAND test_perf_access_2d )
add_test( NAME PerfAccessInterface
	COMMAND test_perf_access_interface )