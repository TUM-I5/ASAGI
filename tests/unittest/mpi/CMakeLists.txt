# SPDX-License-Identifier: LGPLv3
# 
# SPDX-FileCopyrightText: 2015 Sebastian Rettenberger <rettenbs@in.tum.de>

if( NOT THREADSAFE_MPI )
    unittest_add_test( CommThreadUnitTest commthread.h )
endif( NOT THREADSAFE_MPI )
