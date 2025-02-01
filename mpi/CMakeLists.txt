# SPDX-License-Identifier: LGPLv3
# 
# SPDX-FileCopyrightText: 2015 Sebastian Rettenberger <rettenbs@in.tum.de>

##### Files #####
set( mpiSources
    commthread.cpp
    mpicomm.cpp )

##### Add to library #####
add_sources( ${mpiSources} )
