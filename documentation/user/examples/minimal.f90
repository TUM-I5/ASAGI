!>
!! @file
!!  This file is part of ASAGI.
!!
!!  ASAGI is free software: you can redistribute it and/or modify
!!  it under the terms of the GNU Lesser General Public License as
!!  published by the Free Software Foundation, either version 3 of
!!  the License, or  (at your option) any later version.
!!
!!  ASAGI is distributed in the hope that it will be useful,
!!  but WITHOUT ANY WARRANTY; without even the implied warranty of
!!  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
!!  GNU Lesser General Public License for more details.
!!
!!  You should have received a copy of the GNU Lesser General Public
!!  License along with ASAGI.  If not, see <http://www.gnu.org/licenses/>.
!!
!!  Diese Datei ist Teil von ASAGI.
!!
!!  ASAGI ist Freie Software: Sie koennen es unter den Bedingungen
!!  der GNU Lesser General Public License, wie von der Free Software
!!  Foundation, Version 3 der Lizenz oder (nach Ihrer Option) jeder
!!  spaeteren veroeffentlichten Version, weiterverbreiten und/oder
!!  modifizieren.
!!
!!  ASAGI wird in der Hoffnung, dass es nuetzlich sein wird, aber
!!  OHNE JEDE GEWAEHELEISTUNG, bereitgestellt; sogar ohne die implizite
!!  Gewaehrleistung der MARKTFAEHIGKEIT oder EIGNUNG FUER EINEN BESTIMMTEN
!!  ZWECK. Siehe die GNU Lesser General Public License fuer weitere Details.
!!
!!  Sie sollten eine Kopie der GNU Lesser General Public License zusammen
!!  mit diesem Programm erhalten haben. Wenn nicht, siehe
!!  <http://www.gnu.org/licenses/>.
!!
!! @copyright 2015 Sebastian Rettenberger <rettenbs@in.tum.de>

! You have two options:
! - Include the module file _once_ in your project:
!include 'asagi.f90'
! - Compile and link the module file as any other file in your project

!>
!! Fortran Example
!!
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
