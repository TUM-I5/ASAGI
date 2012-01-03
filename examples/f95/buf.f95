include 'asagi.inc'

! Simple library test
program buf
  use asagi
  use, intrinsic :: iso_c_binding
  implicit none
  
  integer :: grid_id
  real, allocatable, target :: value
  
  grid_id = grid_create( GRID_FLOAT )

  if (.not. grid_open( grid_id, "../data/tohoku_1850m_bath.nc" )) then
    write (*,*) 'Could not load file'
    call exit(1)
  end if

  allocate( value )
  call grid_get_buf( grid_id, -1., -5005.32, c_loc( value ) )
  write (*,*) "Value at -1x-5005.32:", value
  
end program buf