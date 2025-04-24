# SPDX-License-Identifier: LGPL-3.0-or-later
# 
# SPDX-FileCopyrightText: 2015 Sebastian Rettenberger <rettenbs@in.tum.de>

##### Files #####
# Simple
set( SimpleCppSources
	simple.cpp )

# Array
set( ArrayCppSources
	array.cpp )

##### Executables #####
# Simple
add_executable( cpp_simple
	${SimpleCppSources} )
target_link_libraries( cpp_simple
	${asagiTarget} ${asagiLibs}
	${MPI_C_LIBRARIES} ${MPI_CXX_LIBRARIES} )
set_target_properties( cpp_simple PROPERTIES
	OUTPUT_NAME simple )

# Array
add_executable( cpp_array
	${ArrayCppSources} )
target_link_libraries( cpp_array
	${asagiTarget} ${asagiLibs}
	${MPI_C_LIBRARIES} ${MPI_CXX_LIBRARIES} )
set_target_properties( cpp_array PROPERTIES
	OUTPUT_NAME array )