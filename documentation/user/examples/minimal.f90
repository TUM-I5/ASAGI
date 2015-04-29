! You have two options:
! - Include the module file _once_ in your project:
!include 'asagi.f90'
! - Compile and link the module file as any other file in your project

program minimal
  use mpi
  use asagi
  use, intrinsic :: iso_c_binding
  implicit none
  
  integer :: grid_id
  real( kind=c_double ), dimension(2) :: pos
  integer :: error

  call MPI_Init( error )
  
  grid_id = asagi_grid_create( )
  call asagi_grid_set_comm( grid_id, MPI_COMM_WORLD )
  ! with threads, set number of threads
  call asagi_grid_set_threads( grid_id, 1 );

  if( asagi_grid_open( grid_id, "/path/to/netcdf/file.nc" ) /= ASAGI_SUCCESS ) then
    write (*,*) "Could not load file"
    call exit(1)
  end if

  pos(:) = 0
  write (*,*) "Value at (0,0):", asagi_grid_get_float( grid_id, pos )

  call asagi_grid_close( grid_id )

  call MPI_Finalize( error )
end program minimal
