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
