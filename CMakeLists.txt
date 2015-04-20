cmake_minimum_required( VERSION 2.8 )

# ASAGI
project( ASAGI )

set( ASAGI_VERSION_MAJOR 0 )
set( ASAGI_VERSION_MINOR 4 )

##### User configuration #####
option( SHARED_LIB "Compile the shared library" ON )
option( STATIC_LIB "Compile the static library" OFF )
option( FORTRAN_SUPPORT "Enable Fortran support" ON )
set( MAX_DIMENSIONS 4 CACHE STRING "Maximum number of dimension supported" )
option( THREADSAFE "Enable support for multiple threads on each node" ON )
option( THREADSAFE_COUNTER "Make performance counter thread safe" OFF )
option( NOMPI "Compile without MPI" OFF )
option( NONUMA "Compile without NUMA support" OFF )
option( THREADSAFE_MPI "Make MPI calls threads safe (required for non threadsafe MPI libraries" ON )
option( EXAMPLES "Compile examples" OFF )
option( TESTS "Test library" OFF )
option( ROUND_ROBIN "Distribute blocks round robin on nodes" ON )
mark_as_advanced( ROUND_ROBIN )
option( DEBUG_NUMA "Overrides CPU NUMA information (each core gets its own NUMA domain" OFF )
mark_as_advanced( DEBUG_NUMA )

##### Default build type #####
if( NOT CMAKE_BUILD_TYPE )
	set( CMAKE_BUILD_TYPE "Release" CACHE STRING
		"Choose the type of build, options are: Debug Release" FORCE)
endif( NOT CMAKE_BUILD_TYPE )

##### Macros #####
# Add sources in subdirectories
macro( add_sources )
    file( RELATIVE_PATH _relPath "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}" )
    foreach( _src ${ARGN} )
        if( _relPath )
            list( APPEND SRCS "${_relPath}/${_src}" )
        else( _relPath )
            list( APPEND SRCS "${_src}" )
        endif( _relPath )
    endforeach()
    
    if( _relPath )
        # propagate SRCS to parent directory
        set( LibSources ${LibSources} ${SRCS} PARENT_SCOPE )
    else( _relPath )
        set( LibSources ${LibSources} ${SRCS} )
    endif( _relPath )
endmacro( add_sources )

# Link static and shared library
macro( lib_link_libraries lib )
    # Link shared lib
    if( SHARED_LIB )
        target_link_libraries( ${lib} ${ARGN} )
    endif( SHARED_LIB )
    
    # Link static lib
    if( STATIC_LIB )
        target_link_libraries( ${lib}-static ${ARGN} )
    endif( STATIC_LIB )
endmacro( lib_link_libraries )

##### Files #####
# Lib Sources
set( mainSources
	asagi.cpp
	asagi_f90.cpp )

# Include files for other programs
set( InterfaceHeaders
	${CMAKE_SOURCE_DIR}/include/asagi.h
	${CMAKE_SOURCE_DIR}/include/asagi.inc )

##### Other configuration #####
# Additional cmake modules
set( CMAKE_MODULE_PATH
	${CMAKE_MODULE_PATH}
	${CMAKE_SOURCE_DIR}/CMake/Modules/ )
	
# Configure debugger
set_property(DIRECTORY PROPERTY COMPILE_DEFINITIONS "DEBUG_PREFIX=\"${CMAKE_PROJECT_NAME} %a %b %d %X\"")
set( CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DDEBUG_LEVEL=0" )

# Enable warnings
add_definitions( -Wall )

# Order include path (solves issues for developers when asagi.{h,inc}
# is already installed)
set( CMAKE_INCLUDE_DIRECTORIES_PROJECT_BEFORE ON )

# Set c++11 standard
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x" )

# Enable fortran
if( FORTRAN_SUPPORT )
	# Call this before searching for mpi
	enable_language( Fortran )
endif()

add_definitions( -DMAX_DIMENSIONS=${MAX_DIMENSIONS} )

# Thread safe?
if( THREADSAFE )
	add_definitions( -DTHREADSAFE )
endif()

# Thead safe counters?
if( THREADSAFE_COUNTER )
    add_definitions( -DTHREADSAFE_COUNTER )
endif()

# Use MPI
if( NOMPI )
	add_definitions( -DASAGI_NOMPI )
else( NOMPI )
	# Find MPI
	find_package( MPI REQUIRED )
	if( MPI_LIBRARY )
		include_directories( SYSTEM ${MPI_C_INCLUDE_PATH} )
	else()
		#Workaround for stupid mpicc/mpicxx
		get_filename_component( MPI_BASE ${MPI_C_COMPILER} PATH )
		set( MPI_BASE ${MPI_BASE}/.. )
		include_directories( SYSTEM ${MPI_BASE}/include )
		link_directories( ${MPI_BASE}/lib )
	endif()
	set( CMAKE_C_COMPILER ${MPI_C_COMPILER} )
	set( CMAKE_CXX_COMPILER ${MPI_CXX_COMPILER} )
	set( CMAKE_Fortran_COMPILER ${MPI_Fortran_COMPILER} ) 
endif( NOMPI )

# Find pthread
if( NONUMA )
    find_package( Threads )
else( NONUMA )
    find_package( Threads REQUIRED )
    find_package( Numa REQUIRED )
    include_directories( SYSTEM ${NUMA_INCLUDE_DIR} )
endif( NONUMA )
if( THREADS_FOUND )
    add_definitions( -DUSE_PTHREAD )
endif( THREADS_FOUND )

# Thread safe MPI?
if( THREADSAFE_MPI )
    add_definitions( -DTHREADSAFE_MPI )
endif( THREADSAFE_MPI )

# Round robin
if( ROUND_ROBIN )
	add_definitions( -DROUND_ROBIN )
endif( ROUND_ROBIN )

# Debug NUMA
if( DEBUG_NUMA )
    add_definitions( -DDEBUG_NUMA )
endif( DEBUG_NUMA )

# Find netcdf
find_package( NetCDF REQUIRED )
include_directories( SYSTEM ${NETCDF_INCLUDES} )

# Find png
find_package( PNG )
if( PNG_FOUND )
	add_definitions( -DPNG_ENABLED )
endif()

# Add library include files for all exectuables
include_directories( ${CMAKE_SOURCE_DIR}/include )

# For sub directories
include_directories( ${CMAKE_SOURCE_DIR} )

# Utils
include_directories( ${CMAKE_SOURCE_DIR}/submodules )

# Add sources in subdirectories
add_subdirectory( allocator )
add_subdirectory( cache )
add_subdirectory( fortran )
add_subdirectory( grid )
add_subdirectory( io )
add_subdirectory( perf )
add_subdirectory( magic )
if( NOT NOMPI )
	add_subdirectory( mpi )
endif( NOT NOMPI )
add_subdirectory( numa )
add_subdirectory( threads )
add_subdirectory( transfer )
add_subdirectory( types )

# Add sources
add_sources( ${mainSources} )

# Set rpath
# use, i.e. don't skip the full RPATH for the build tree
SET(CMAKE_SKIP_BUILD_RPATH  FALSE)
# when building, don't use the install RPATH already
# (but later on when installing)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE) 
# the RPATH to be used when installing, but only if it's not a system directory
LIST(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)
IF("${isSystemDir}" STREQUAL "-1")
   SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
ENDIF("${isSystemDir}" STREQUAL "-1")
# add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# Documentation
add_subdirectory( documentation )

##### Libraries #####
# Libraries (static and shared)
if( SHARED_LIB )
    add_library( asagi SHARED ${LibSources} )
endif( SHARED_LIB )
if( STATIC_LIB )
    add_library( asagi-static STATIC ${LibSources} )
    set_target_properties( asagi-static
        PROPERTIES OUTPUT_NAME asagi )
endif( STATIC_LIB )

# Set static and shared libary output name to "nompi"
if( NOMPI )
    # Shared library
    if( SHARED_LIB )
	    set_target_properties( asagi
		    PROPERTIES OUTPUT_NAME asagi_nompi )
    endif( SHARED_LIB )
		
    # Static library
    if( STATIC_LIB )
        set_target_properties( asagi-static
            PROPERTIES OUTPUT_NAME asagi_nompi )
    endif( STATIC_LIB )
endif( NOMPI )

# Set the target that can be used to link programs with ASAGI
if( SHARED_LIB )
    set( asagiTarget asagi )
else( SHARED_LIB )
    # We assume at least one of shared/static is build
    set( asagiTarget asagi-static )
endif( SHARED_LIB )

# Additional libs required to link with asagi
set( asagiLibs ${NETCDF_LIBRARIES} )

if( THREAD_FOUND )
    set( asagiLibs ${asagiLibs} ${CMAKE_THREAD_LIBS_INIT} )
endif( THREAD_FOUND )
if( NOT NONUMA )
    link_directories( ${NUMA_LIBRARY_DIR} )
    set( asagiLibs ${asagiLibs} ${NUMA_LIBRARY} )
endif( NOT NONUMA )

##### Excecutables #####
# Tests
if( TESTS )
	enable_testing()
	add_subdirectory( tests )
endif( TESTS )

# Examples (only with MPI)
if( NOT NOMPI )
	if( EXAMPLES )
		# Add examples
		add_subdirectory( examples )
	endif( EXAMPLES )
endif( NOT NOMPI )

##### Installation #####
if( SHARED_LIB )
    install( TARGETS asagi DESTINATION lib )
endif( SHARED_LIB )
if( STATIC_LIB )
    install( TARGETS asagi-static DESTINATION lib )
endif( STATIC_LIB )
install( FILES include/asagi.h include/asagi.f90
	DESTINATION include )


##### Packaging #####
INCLUDE(InstallRequiredSystemLibraries)

SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${CMAKE_PROJECT_NAME})
SET(CPACK_PACKAGE_VENDOR "Sebastian Rettenberger")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")
SET(CPACK_PACKAGE_VERSION_MAJOR ${ASAGI_VERSION_MAJOR})
SET(CPACK_PACKAGE_VERSION_MINOR ${ASAGI_VERSION_MINOR})
SET(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_SOURCE_IGNORE_FILES
	"\\\\.svn/"
	"\\\\.git/"
	"\\\\.kdev4"
	"~$"
	"/preprocess/"
	"/test_progs/"
)

SET(CPACK_PACKAGE_INSTALL_DIRECTORY "CMake ${CMake_VERSION_MAJOR}.${CMake_VERSION_MINOR}")
INCLUDE(CPack)