!>
!! SPDX-License-Identifier: LGPL-3.0-or-later
!!
!! SPDX-FileCopyrightText: 2015 Sebastian Rettenberger <rettenbs@in.tum.de>


program fortran2d
  use asagi
  use mpi
  use, intrinsic :: iso_c_binding
  implicit none
  
  integer :: i, j
  integer :: grid_id
  integer :: error
  real( kind=c_double ), dimension(2) :: pos
  integer :: value

  call MPI_Init( error )
  
  grid_id = asagi_grid_create( ASAGI_FLOAT )

  call asagi_grid_set_comm( grid_id, MPI_COMM_WORLD )

  if( asagi_grid_open( grid_id, "2dgrid.nc" ) /= ASAGI_SUCCESS ) then
    write (*,*) 'Could not load file'
    call exit(1)
  end if

  do i=0,69
    pos(1) = i

    do j=0,69
        pos(2) = j

        value = j + i * 70
        if( asagi_grid_get_int( grid_id, pos) /= value ) then
            write (*,*) "Value at", i, j, "should be", value, "but is", asagi_grid_get_int( grid_id, pos )
            call exit(1)
        endif
    enddo
  enddo

  call asagi_grid_close( grid_id )

  call MPI_Finalize( error )
  
end program fortran2d
