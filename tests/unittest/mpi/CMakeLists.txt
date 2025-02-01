# SPDX-License-Identifier: LGPL-3.0-or-later
# 
# SPDX-FileCopyrightText: 2015 Sebastian Rettenberger <rettenbs@in.tum.de>

if( NOT THREADSAFE_MPI )
    unittest_add_test( CommThreadUnitTest commthread.h )
endif( NOT THREADSAFE_MPI )
