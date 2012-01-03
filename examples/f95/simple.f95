include 'asagi.inc'

! Simple library test
program simple
  use asagi
  implicit none
  
  integer :: grid_id
  
  grid_id = grid_create( GRID_FLOAT )

  if (.not. grid_open( grid_id, "../data/tohoku_1850m_bath.nc" )) then
    write (*,*) 'Could not load file'
    call exit(1)
  end if
  

  write (*,*) "Range X:", grid_min_x( grid_id ), grid_max_x( grid_id )
  write (*,*) "Range Y:", grid_min_y( grid_id ), grid_max_y( grid_id )

  write (*,*) "Value at 5x10:", grid_get_float( grid_id, 5., 10. )
  write (*,*) "Value at 5x10.1:", grid_get_float( grid_id, 5., 10.1 )
  write (*,*) "Value at -1x-5005.32:", grid_get_float( grid_id, -1., -5005.32 )
  
end program simple