# SPDX-License-Identifier: LGPLv3
# 
# SPDX-FileCopyrightText: 2015 Sebastian Rettenberger <rettenbs@in.tum.de>

# Add all test directories
add_subdirectory( c )
add_subdirectory( cpp )
if( FORTRAN_SUPPORT )
	add_subdirectory( f90 )
endif()
