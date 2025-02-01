# SPDX-License-Identifier: LGPL-3.0-or-later
# 
# SPDX-FileCopyrightText: 2015 Sebastian Rettenberger <rettenbs@in.tum.de>

# Add all test directories
add_subdirectory( c )
add_subdirectory( cpp )
if( FORTRAN_SUPPORT )
	add_subdirectory( f90 )
endif()
