# SPDX-License-Identifier: LGPLv3
# 
# SPDX-FileCopyrightText: 2015 Sebastian Rettenberger <rettenbs@in.tum.de>

# Examples (only with MPI)
if( NOT NOMPI )
	if( EXAMPLES )
		# Add examples
		add_subdirectory( examples )
	endif( EXAMPLES )
endif( NOT NOMPI )