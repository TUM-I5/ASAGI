# SPDX-License-Identifier: LGPLv3
# 
# SPDX-FileCopyrightText: 2015 Sebastian Rettenberger <rettenbs@in.tum.de>

##### Files #####
if( THREADS_FOUND )
    set( numaSources
	    numa.cpp )
else( THREADS_FOUND )
    set( numaSources
        nonuma.cpp )
endif( THREADS_FOUND )

##### Add to library #####
add_sources( ${numaSources} )