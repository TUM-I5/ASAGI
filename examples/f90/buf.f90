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

  if( grid_open( grid_id, "../data/tohoku_1850m_bath.nc" ) /= GRID_SUCCESS ) then
    write (*,*) 'Could not load file'
    call exit(1)
  end if

  allocate( value )
  call grid_get_buf_2d( grid_id, c_loc( value ), -1.d+0, -5005.32d+0 )
  write (*,*) "Value at -1x-5005.32:", value

  call grid_close( grid_id )

  call MPI_Finalize( error )
  
end program buf
