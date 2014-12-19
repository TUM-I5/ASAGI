cmake_minimum_required( VERSION 2.8 )

# ASAGI
project( ASAGI )

set( ASAGI_VERSION_MAJOR 0 )
set( ASAGI_VERSION_MINOR 3 )

##### User configuration #####
option( SHARED_LIB "Compile the shared library" ON )
option( STATIC_LIB "Compile the static library" OFF )
option( THREADSAFETY "Enable support for multiple threads on each node" ON )
option( FORTRAN_SUPPORT "Enable Fortran support" ON )
option( EXAMPLES "Compile examples" OFF )
option( TESTS "Test library" OFF )
option( NOMPI "Compile without MPI" OFF )
option( ROUND_ROBIN "Distribute blocks round robin on nodes (internal use)" ON )

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

# Set static and shared libary output name
macro( set_lib_output_name lib name)
    # Shared library
    if( SHARED_LIB )
	    set_target_properties( ${lib}
		    PROPERTIES OUTPUT_NAME ${name} )
    endif( SHARED_LIB )
		
    # Static library
    if( STATIC_LIB )
        set_target_properties( ${lib}-static
            PROPERTIES OUTPUT_NAME ${name} )
    endif( STATIC_LIB )
endmacro( set_lib_output_name )

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

# Thread-safety
if( THREADSAFETY )
	add_definitions( -DTHREADSAFETY )
endif()

# Use MPI
if( NOMPI )
	add_definitions( -DASAGI_NOMPI )
endif( NOMPI )

# Round robin
if( ROUND_ROBIN )
	add_definitions( -DROUND_ROBIN )
endif( ROUND_ROBIN )

if( NOT NOMPI )
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
endif( NOT NOMPI )

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
add_subdirectory( blocks )
add_subdirectory( fortran )
add_subdirectory( grid )
add_subdirectory( io )
add_subdirectory( perf )
if( NOT NOMPI )
	add_subdirectory( mpi )
endif( NOT NOMPI )
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

# Set the target that can be used to link programs with ASAGI
if( SHARED_LIB )
    set( asagiTarget asagi )
else( SHARED_LIB )
    # We assume at least one of shared/static is build
    set( asagiTarget asagi-static )
endif( SHARED_LIB )

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
	endif()
endif( NOT NOMPI )

##### Excecutables #####
# Libraries (static and shared)
if( SHARED_LIB )
    add_library( asagi SHARED ${LibSources} )
endif( SHARED_LIB )
if( STATIC_LIB )
    add_library( asagi-static STATIC ${LibSources} )
    set_target_properties( asagi-static
        PROPERTIES OUTPUT_NAME asagi )
endif( STATIC_LIB )

if( NOMPI )
	set_lib_output_name( asagi asagi_nompi )
else( NOMPI )
	lib_link_libraries( asagi )
endif( NOMPI )

lib_link_libraries( asagi ${NETCDF_LIBRARIES} )
if( PNG_FOUND )
	lib_link_libraries( asagi ${PNG_LIBRARY} )
endif( PNG_FOUND )

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
#SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/ReadMe.txt")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")
SET(CPACK_PACKAGE_VERSION_MAJOR ${ASAGI_VERSION_MAJOR})
SET(CPACK_PACKAGE_VERSION_MINOR ${ASAGI_VERSION_MINOR})
SET(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_SOURCE_IGNORE_FILES
	"\\\\.svn/"
	"\\\\.kdev4"
	"~$"
	"/preprocess/"
	"/test_progs/"
)

SET(CPACK_PACKAGE_INSTALL_DIRECTORY "CMake ${CMake_VERSION_MAJOR}.${CMake_VERSION_MINOR}")
INCLUDE(CPack)