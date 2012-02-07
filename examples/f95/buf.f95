include 'asagi.inc'

! Simple library test
program buf
  use asagi
  use mpi
  use, intrinsic :: iso_c_binding
  implicit none
  
  integer :: grid_id
  integer :: error
  real, allocatable, target :: value

  call MPI_Init( error )
  
  grid_id = grid_create( GRID_FLOAT )

  if (.not. grid_open( grid_id, "../data/tohoku_1850m_bath.nc", MPI_COMM_WORLD )) then
    write (*,*) 'Could not load file'
    call exit(1)
  end if

  allocate( value )
  call grid_get_buf( grid_id, -1., -5005.32, c_loc( value ) )
  write (*,*) "Value at -1x-5005.32:", value

  call grid_free( grid_id )

  call MPI_Finalize( error )
  
end program buf