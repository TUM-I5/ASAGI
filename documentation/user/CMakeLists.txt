# Examples (only with MPI)
if( NOT NOMPI )
	if( EXAMPLES )
		# Add examples
		add_subdirectory( examples )
	endif( EXAMPLES )
endif( NOT NOMPI )