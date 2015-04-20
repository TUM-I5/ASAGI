! Simple library test
program simple
  use asagi
  use mpi
  use, intrinsic :: iso_c_binding
  implicit none
  
  integer :: grid_id
  integer :: error
  real( kind=c_double ), dimension(2) :: pos

  call MPI_Init( error )
  
  grid_id = asagi_grid_create( ASAGI_FLOAT )

  if( asagi_grid_open( grid_id, "tests/2dgrid.nc" ) /= ASAGI_SUCCESS ) then
    write (*,*) 'Could not load file'
    call exit(1)
  end if
  
  call asagi_grid_set_comm( grid_id, MPI_COMM_WORLD )

  write (*,*) "Range X:", asagi_grid_min( grid_id, 0 ), asagi_grid_max( grid_id, 0 )
  write (*,*) "Range Y:", asagi_grid_min( grid_id, 1 ), asagi_grid_max( grid_id, 1 )

  pos(1) = 5.0
  pos(2) = 10.0
  write (*,*) "Value at 5x10:", asagi_grid_get_float( grid_id, pos )
  pos(1) = 5.0
  pos(2) = 10.1
  write (*,*) "Value at 5x10.1:", asagi_grid_get_float( grid_id, pos )
  pos(1) = -1.0
  pos(2) = -5005.32
  write (*,*) "Value at -1x-5005.32:", asagi_grid_get_float( grid_id, pos )

  call asagi_grid_close( grid_id )

  call MPI_Finalize( error )
  
end program simple
