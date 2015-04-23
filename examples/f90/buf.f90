! Simple library test
program buf
  use asagi
  use mpi
  use, intrinsic :: iso_c_binding
  implicit none
  
  integer :: grid_id
  integer :: error
  real( kind=c_double ), dimension(2) :: pos
  real, allocatable, target :: value

  call MPI_Init( error )
  
  grid_id = asagi_grid_create( ASAGI_FLOAT )

  call asagi_grid_set_comm( grid_id, MPI_COMM_WORLD )

  if( asagi_grid_open( grid_id, "tests/2dgrid.nc" ) /= ASAGI_SUCCESS ) then
    write (*,*) 'Could not load file'
    call exit(1)
  end if

  allocate( value )
  pos(1) = 1.0
  pos(2) = 50.3
  call asagi_grid_get_buf( grid_id, c_loc( value ), pos )
  write (*,*) "Value at 1x50.3:", value

  call asagi_grid_close( grid_id )

  call MPI_Finalize( error )
  
end program buf
