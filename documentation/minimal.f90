include 'asagi.inc'

program minimal
  use mpi
  use asagi
  implicit none
  
  integer :: grid_id
  integer :: error

  call MPI_Init( error )
  
  grid_id = grid_create( )

  if( grid_open( grid_id, "/path/to/netcdf/file.nc" ) /= GRID_SUCCESS ) then
    write (*,*) "Could not load file"
    call exit(1)
  end if

  write (*,*) "Value at (0,0):", grid_get_float( grid_id, 0.d+0, 0.d+0 )

  call grid_close( grid_id )

  call MPI_Finalize( error )
end program minimal