# SPDX-License-Identifier: LGPLv3
# 
# SPDX-FileCopyrightText: 2015 Sebastian Rettenberger <rettenbs@in.tum.de>

##### Files #####
set( gridSources
	grid.cpp )

##### Add subdirectory #####
add_subdirectory( level )

##### Add to library #####
add_sources( ${gridSources} )