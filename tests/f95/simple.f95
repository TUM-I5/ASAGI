include 'database.f95'

! Simple library test
program simple
  use database
  implicit none
  
  integer grid_id

  grid_id = grid_load( "Test" )
  
  if (grid_id < 0) then
    write (*,*) 'Could not load file'
    call exit(1)
  end if
end program simple