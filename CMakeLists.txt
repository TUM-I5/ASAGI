# @file
#  This file is part of ASAGI.
# 
#  ASAGI is free software: you can redistribute it and/or modify
#  it under the terms of the GNU Lesser General Public License as
#  published by the Free Software Foundation, either version 3 of
#  the License, or  (at your option) any later version.
#
#  ASAGI is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with ASAGI.  If not, see <http://www.gnu.org/licenses/>.
#
#  Diese Datei ist Teil von ASAGI.
#
#  ASAGI ist Freie Software: Sie koennen es unter den Bedingungen
#  der GNU Lesser General Public License, wie von der Free Software
#  Foundation, Version 3 der Lizenz oder (nach Ihrer Option) jeder
#  spaeteren veroeffentlichten Version, weiterverbreiten und/oder
#  modifizieren.
#
#  ASAGI wird in der Hoffnung, dass es nuetzlich sein wird, aber
#  OHNE JEDE GEWAEHELEISTUNG, bereitgestellt; sogar ohne die implizite
#  Gewaehrleistung der MARKTFAEHIGKEIT oder EIGNUNG FUER EINEN BESTIMMTEN
#  ZWECK. Siehe die GNU Lesser General Public License fuer weitere Details.
#
#  Sie sollten eine Kopie der GNU Lesser General Public License zusammen
#  mit diesem Programm erhalten haben. Wenn nicht, siehe
#  <http://www.gnu.org/licenses/>.
# 
# @copyright 2015 Sebastian Rettenberger <rettenbs@in.tum.de>

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
option( THREADSAFE_COUNTER "Make access counters thread-safe" OFF )
option( THREADSAFE_MPI "Make MPI calls thread-safe (required for non thread-safe MPI libraries)" ON )
option( NOMPI "Compile without MPI" OFF )
option( NONUMA "Compile without NUMA support" OFF )
option( TESTS "Enable tests" OFF )
option( EXAMPLES "Compile examples" OFF )
option( ROUND_ROBIN "Distribute blocks round robin on nodes" ON )
mark_as_advanced( ROUND_ROBIN )
option( DEBUG_NUMA "Overrides CPU NUMA information (each core gets its own NUMA domain" OFF )
mark_as_advanced( DEBUG_NUMA )

##### Default build type #####
if( NOT CMAKE_BUILD_TYPE )
	set( CMAKE_BUILD_TYPE "Release" CACHE STRING
		"Choose the type of build, options are: Debug Release" FORCE )
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
    add_definitions( -DASAGI_NONUMA )
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

# List libraries required by ASAGI (and library directories)
set( asagiLibs ${NETCDF_LIBRARIES} )

if( THREADS_FOUND )
    set( asagiLibs ${asagiLibs} ${CMAKE_THREAD_LIBS_INIT} )
endif( THREADS_FOUND )
if( NOT NONUMA )
    set( asagiLibs ${asagiLibs} ${NUMA_LIBRARY} )
endif( NOT NONUMA )

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

##### Libraries #####
# Set name for the library file
if( NOMPI )
    set( asagiOutputName "asagi_nompi" )
else( NOMPI )
    set( asagiOutputName "asagi" )
endif( NOMPI )

# Libraries (static and shared)
if( SHARED_LIB )
    add_library( asagi SHARED ${LibSources} )
    target_link_libraries( asagi ${asagiLibs} )
    set_target_properties( asagi
        PROPERTIES OUTPUT_NAME ${asagiOutputName}
        VERSION ${ASAGI_VERSION_MAJOR}.${ASAGI_VERSION_MINOR}
        SOVERSION ${ASAGI_VERSION_MAJOR} )
    
    # Unset asagiLibs since we already linked them
    unset( asagiLibs )
endif( SHARED_LIB )
if( STATIC_LIB )
    add_library( asagi-static STATIC ${LibSources} )
    set_target_properties( asagi-static
        PROPERTIES OUTPUT_NAME ${asagiOutputName} )
endif( STATIC_LIB )

# Set the target that can be used to link programs with ASAGI
# and ink libraries
if( SHARED_LIB )
    set( asagiTarget asagi )
else( SHARED_LIB )
    # We assume at least one of shared/static is build
    set( asagiTarget asagi-static )
endif( SHARED_LIB )

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

##### Documentation #####
# Documentation and examples included in the doc 
add_subdirectory( documentation )

##### pkg-config #####
find_package( PkgConfig )
if( PKGCONFIG_FOUND )
    # Detect private libs and directories

    set( _PKG_CONFIG_PROJECT_NAME ${PROJECT_NAME} )
    set( _PKG_CONFIG_DESCRIPTION "a pArallel Server for Adaptive GeoInformation" )
    set( _PKG_CONFIG_URL "https://github.com/TUM-I5/ASAGI" )
    set( _PKG_CONFIG_VERSION ${ASAGI_VERSION_MAJOR}.${ASAGI_VERSION_MINOR} )
    
    set( _PKG_CONFIG_PREFIX ${CMAKE_INSTALL_PREFIX} )
    set( _PKG_CONFIG_LIBDIR "\${prefix}/lib" )
    set( _PKG_CONFIG_INCLUDEDIR "\${prefix}/include" )

    set( _PKG_CONFIG_REQUIRES "" )
    set( _PKG_CONFIG_REQUIRES_PRIVATE "netcdf" )
    set( _PKG_CONFIG_CONFLICTS "" )
    set( _PKG_CONFIG_LIBS "${CMAKE_LIBRARY_PATH_FLAG}\${libdir} ${CMAKE_LINK_LIBRARY_FLAG}${asagiOutputName}" )
    
    if( NOT NONUMA )
        set( _PKG_CONFIG_LIBS_PRIVATE "${CMAKE_LINK_LIBRARY_FLAG}numa" )
        set( _PKG_CONFIG_LIBDIRS_PRIVATE ${CMAKE_LIBRARY_PATH_FLAG}${NUMA_LIBRARY_DIR} )
    endif( NOT NONUMA )
    set( _PKG_CONFIG_LIBS_PRIVATE  "${_PKG_CONFIG_LIBDIRS_PRIVATE} ${_PKG_CONFIG_LIBS_PRIVATE}" )
    set( _PKG_CONFIG_CFLAGS "-I\${includedir}" )

    # Where to install the pkg-config file?
    set( _PKG_CONFIG_DIR ${CMAKE_INSTALL_PREFIX}/lib/pkgconfig )
    
    # Configure the pkgconfig file
    string( TOLOWER "${asagiOutputName}.pc" _PKG_FILENAME )
    configure_file( CMake/pkg-config.pc.in ${_PKG_FILENAME} )
    
    # Install the pkg file
    install( FILES ${CMAKE_CURRENT_BINARY_DIR}/${_PKG_FILENAME}	DESTINATION lib/pkgconfig )
endif( PKGCONFIG_FOUND )

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
include( InstallRequiredSystemLibraries )

SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${CMAKE_PROJECT_NAME})
SET(CPACK_PACKAGE_VENDOR "Sebastian Rettenberger")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")
SET(CPACK_PACKAGE_VERSION_MAJOR ${ASAGI_VERSION_MAJOR})
SET(CPACK_PACKAGE_VERSION_MINOR ${ASAGI_VERSION_MINOR})
SET(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_SOURCE_IGNORE_FILES
	"\\\\.git/"
	"\\\\.kdev4"
	"~$"
	"/preprocess/"
	"/test_progs/"
)

SET(CPACK_PACKAGE_INSTALL_DIRECTORY "CMake ${CMake_VERSION_MAJOR}.${CMake_VERSION_MINOR}")
include( CPack )