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
!! @copyright 2012-2015 Sebastian Rettenberger <rettenbs@in.tum.de>
!!
!! @brief Include file for Fortran API
!!
!! @defgroup f_interface Fortran Interface

!> @ingroup f_interface
!!
!! ASAGI Fortran Interface
module asagi
  implicit none

  !> @cond ignore
  !! currently not supported with Doxygen
  !> @ingroup f_interface
  !!
  !! @see asagi::Grid::Type
  !! @endcond ignore
  enum, bind( c )
    enumerator :: ASAGI_BYTE, ASAGI_INT, ASAGI_LONG, ASAGI_FLOAT, ASAGI_DOUBLE
  end enum
  
  !> @cond ignore
  !! currently not supported with Doxygen
  !> @ingroup f_interface
  !!
  !! @see asagi::Grid::Error
  !! @endcond ignore
  enum, bind( c )
    enumerator :: &
      ASAGI_SUCCESS = 0, &
      ASAGI_MPI_ERROR, &
      ASAGI_THREAD_ERROR, &
      ASAGI_NUMA_ERROR, &
      ASAGI_UNKNOWN_PARAM, &
      ASAGI_INVALID_VALUE, &
      ASAGI_NOT_INITIALIZED, &
      ASAGI_ALREADY_INITIALIZED, &
      ASAGI_NOT_OPEN, &
      ASAGI_VAR_NOT_FOUND, &
      ASAGI_WRONG_SIZE, &
      ASAGI_UNSUPPORTED_DIMENSIONS, &
      ASAGI_INVALID_VAR_SIZE
  end enum
  
  !> @cond ingore
  interface
  !> @endcond ingore
  
    !> @internal
    function asagi_grid_create_c( type ) bind( c, name="f90asagi_grid_create" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value :: type
      integer( kind=c_int )        :: asagi_grid_create_c
    end function asagi_grid_create_c

    !> @internal
    function asagi_grid_create_array_c( basictype ) bind( c, name="f90asagi_grid_create_array" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value :: basictype
      integer( kind=c_int )        :: asagi_grid_create_array_c
    end function asagi_grid_create_array_c

    !> @ingroup f_interface
    !!
    !! @see asagi::Grid::createStruct()
    function asagi_grid_create_struct( count, block_length, displacments, types ) bind( c, name="f90asagi_grid_create_struct" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value                     :: count
      integer( kind=c_int ), dimension(*), intent(in)  :: block_length
      integer( kind=c_long ), dimension(*), intent(in) :: displacments
      integer( kind=c_int ), dimension(*), intent(in)  :: types
      integer( kind=c_int )                            :: asagi_grid_create_struct
    end function asagi_grid_create_struct

    !> @ingroup f_interface
    !!
    !! @see asagi::Grid::setComm()
    subroutine asagi_grid_set_comm( grid_id, comm ) bind( c, name="f90asagi_grid_set_comm" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value :: grid_id
      integer( kind=c_int ), value :: comm
    end subroutine asagi_grid_set_comm

    !> @ingroup f_interface
    !!
    !! @see asagi::Grid::setThreads()
    subroutine asagi_grid_set_threads( grid_id, threads ) bind( c, name="f90asagi_grid_set_threads" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value :: grid_id
      integer( kind=c_int ), value :: threads
    end subroutine asagi_grid_set_threads

    !> @internal
    subroutine asagi_grid_set_param_c( grid_id, name, value, level ) bind( c, name="f90asagi_grid_set_param" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value                       :: grid_id
      character( kind=c_char ), dimension(*), intent(in) :: name
      character( kind=c_char ), dimension(*), intent(in) :: value
      integer( kind=c_int ), value                       :: level
    end subroutine asagi_grid_set_param_c
    
    !> @internal
    function asagi_grid_open_c( grid_id, filename, level ) bind( c, name="f90asagi_grid_open" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value                       :: grid_id
      character( kind=c_char ), dimension(*), intent(in) :: filename
      integer( kind=c_int ), value                       :: level
      integer( kind=c_int )                              :: asagi_grid_open_c
    end function asagi_grid_open_c
    
    !> @ingroup f_interface
    !!
    !! @see agagi::Grid::getDimensions()
    function asagi_grid_dimensions( grid_id ) bind( c, name="f90asagi_grid_dimensions" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value :: grid_id
      integer( kind=c_int )        :: asagi_grid_dimensions
    end function asagi_grid_dimensions

    !> @ingroup f_interface
    !!
    !! @see asagi::Grid::getMin()
    function asagi_grid_min( grid_id, n ) bind( c, name="f90asagi_grid_min" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value :: grid_id
      integer( kind=c_int ), value :: n
      real( kind=c_double )        :: asagi_grid_min
    end function asagi_grid_min
    
    !> @ingroup f_interface
    !!
    !! @see asagi::Grid::getMax()
    function asagi_grid_max( grid_id, n ) bind( c, name="f90asagi_grid_max" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value :: grid_id
      integer( kind=c_int ), value :: n
      real( kind=c_double )        :: asagi_grid_max
    end function asagi_grid_max

    !> @internal
    function asagi_grid_delta_c( grid_id, n, level ) bind( c, name="f90asagi_grid_delta" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value :: grid_id
      integer( kind=c_int ), value :: n
      integer( kind=c_int ), value :: level
      real( kind=c_double )        :: asagi_grid_delta_c
    end function asagi_grid_delta_c

    !> @ingroup f_interface
    !!
    !! @see asagi::Grid::getVarSize()
    function asagi_grid_var_size( grid_id ) bind( c, name="f90asagi_grid_var_size" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value :: grid_id
      integer( kind=c_int )        :: asagi_grid_var_size
    end function asagi_grid_var_size
    
    !> @internal
    function asagi_grid_get_byte_c( grid_id, pos, level ) bind( c, name="f90asagi_grid_get_byte" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value                    :: grid_id
      real( kind=c_double ), dimension(*), intent(in) :: pos
      integer( kind=c_int ), value                    :: level
      character( kind=c_char )                        :: asagi_grid_get_byte_c
    end function asagi_grid_get_byte_c
    
    !> @internal
    function asagi_grid_get_int_c( grid_id, pos, level ) bind( c, name="f90asagi_grid_get_int" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value                    :: grid_id
      real( kind=c_double ), dimension(*), intent(in) :: pos
      integer( kind=c_int ), value                    :: level
      integer( kind=c_int )                           :: asagi_grid_get_int_c
    end function asagi_grid_get_int_c
    
    !> @internal
    function asagi_grid_get_long_c( grid_id, pos, level ) bind( c, name="f90asagi_grid_get_long" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value                    :: grid_id
      real( kind=c_double ), dimension(*), intent(in) :: pos
      integer( kind=c_int ), value                    :: level
      integer( kind=c_long )                          :: asagi_grid_get_long_c
    end function asagi_grid_get_long_c
    
    !> @internal
    function asagi_grid_get_float_c( grid_id, pos, level ) bind( c, name="f90asagi_grid_get_float" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value                    :: grid_id
      real( kind=c_double ), dimension(*), intent(in) :: pos
      integer( kind=c_int ), value                    :: level
      real( kind=c_float )                            :: asagi_grid_get_float_c
    end function asagi_grid_get_float_c
    
    !> @internal
    function asagi_grid_get_double_c( grid_id, pos, level ) bind( c, name="f90asagi_grid_get_double" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value                    :: grid_id
      real( kind=c_double ), dimension(*), intent(in) :: pos
      integer( kind=c_int ), value                    :: level
      real( kind=c_double )                           :: asagi_grid_get_double_c
    end function asagi_grid_get_double_c
    
    !> @internal
    subroutine asagi_grid_get_buf_c( grid_id, buf, pos, level ) bind( c, name="f90asagi_grid_get_buf" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value                    :: grid_id
      type( c_ptr ), value                            :: buf
      real( kind=c_double ), dimension(*), intent(in) :: pos
      integer( kind=c_int ), value                    :: level
    end subroutine asagi_grid_get_buf_c
    
    !> @ingroup f_interface
    !!
    !! @see asagi::Grid#close(asagi::Grid*)
    subroutine asagi_grid_close( grid_id ) bind( c, name="f90asagi_grid_close" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value :: grid_id
    end subroutine asagi_grid_close

    !> @ingroup f_interface
    !!
    !! @see asagi::Grid::startCommThread
    function asagi_start_comm_thread( sched_cpu, comm ) bind( c, name="f90asagi_start_comm_thread" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value :: sched_cpu
      integer( kind=c_int ), value :: comm
      integer( kind=c_int )        :: asagi_start_comm_thread
    end function asagi_start_comm_thread

    !> @ingroup f_interface
    !!
    !! @see asagi::Grid::stopCommThread
    subroutine asagi_stop_comm_thread() bind( c, name="f90asagi_stop_comm_thread" )
      use, intrinsic :: iso_c_binding
    end subroutine asagi_stop_comm_thread

    !> @ingroup f_interface
    !!
    !! @see asagi::Grid::nodeLocalRank
    function asagi_node_local_rank( comm ) bind( c, name="f90asagi_node_local_rank" )
        use, intrinsic :: iso_c_binding
        integer( kind=c_int ), value :: comm
        integer( kind=c_int )        :: asagi_node_local_rank
    end function
  !> @cond ignore
  end interface
  !> @endcond ignore

  contains
    !> @ingroup f_interface
    !!
    !! @see asagi::Grid::create()
    function asagi_grid_create( type )
      integer, optional, intent(in) :: type
      integer                       :: asagi_grid_create

      !variables send to asagi
      integer :: t

      if( present( type ) ) then
        t = type
      else
        t = ASAGI_FLOAT
      endif

      asagi_grid_create = asagi_grid_create_c( t )
    end function asagi_grid_create

    !> @ingroup f_interface
    !!
    !! @see asagi::Grid::createArray()
    function asagi_grid_create_array( basictype )
      integer, optional, intent(in) :: basictype
      integer                       :: asagi_grid_create_array

      !variables send to asagi
      integer :: t

      if( present( basictype ) ) then
        t = basictype
      else
        t = ASAGI_FLOAT
      endif

      asagi_grid_create_array = asagi_grid_create_array_c( t )
    end function asagi_grid_create_array

    !> @ingroup f_interface
    !!
    !! @see asagi::Grid::setParam()
    subroutine asagi_grid_set_param( grid_id, name, value, level )
      use, intrinsic :: iso_c_binding
      integer, intent(in)           :: grid_id
      character*(*), intent(in)     :: name
      character*(*), intent(in)     :: value
      integer, optional, intent(in) :: level

      integer :: l !level send to asagi

      if( present( level ) ) then
        l = level
      else
        l = 0
      endif

      call asagi_grid_set_param_c( grid_id, name // c_null_char, &
        value // c_null_char, l )
    end subroutine asagi_grid_set_param

    !> @ingroup f_interface
    !!
    !! @see asagi::Grid::open()
    function asagi_grid_open( grid_id, filename, level )
      use, intrinsic :: iso_c_binding
      integer, intent(in)           :: grid_id
      character*(*), intent(in)     :: filename
      integer, optional, intent(in) :: level
      integer                       :: asagi_grid_open

      integer :: l !level send to asagi

      if( present( level ) ) then
        l = level
      else
        l = 0
      endif

      asagi_grid_open = asagi_grid_open_c( grid_id, filename // c_null_char, l )
    end function asagi_grid_open
    
    !> @ingroup f_interface
    !!
    !! @see asagi::Grid::getDelta()
    function asagi_grid_delta( grid_id, n, level )
      use, intrinsic :: iso_c_binding
      integer, intent(in)           :: grid_id
      integer, intent(in)           :: n
      integer, optional, intent(in) :: level
      real( kind=c_double )         :: asagi_grid_delta

      integer :: l !level send to asagi

      if( present( level ) ) then
        l = level
      else
        l = 0
      endif

      asagi_grid_delta = asagi_grid_delta_c( grid_id, n, l )
    end function asagi_grid_delta

    !> @ingroup f_interface
    !!
    !! @see asagi::Grid::getByte()
    function asagi_grid_get_byte( grid_id, pos, level )
      use, intrinsic :: iso_c_binding
      integer, intent(in)                             :: grid_id
      real( kind=c_double ), dimension(*), intent(in) :: pos
      integer, optional, intent(in)                   :: level
      character                                       :: asagi_grid_get_byte

      integer :: l !level send to asagi

      if( present( level ) ) then
        l = level
      else
        l = 0
      endif

      asagi_grid_get_byte = asagi_grid_get_byte_c( grid_id, pos, l )
    end function asagi_grid_get_byte
    
    !> @ingroup f_interface
    !!
    !! @see asagi::Grid::getInt()
    function asagi_grid_get_int( grid_id, pos, level )
      use, intrinsic :: iso_c_binding
      integer, intent(in)                             :: grid_id
      real( kind=c_double ), dimension(*), intent(in) :: pos
      integer, optional, intent(in)                   :: level
      integer                                         :: asagi_grid_get_int

      integer :: l !level send to asagi

      if( present( level ) ) then
        l = level
      else
        l = 0
      endif

      asagi_grid_get_int = asagi_grid_get_int_c( grid_id, pos, l )
    end function asagi_grid_get_int
    
    !> @ingroup f_interface
    !!
    !! @see asagi::Grid::getLong()
    function asagi_grid_get_long( grid_id, pos, level )
      use, intrinsic :: iso_c_binding
      integer, intent(in)                             :: grid_id
      real( kind=c_double ), dimension(*), intent(in) :: pos
      integer, optional, intent(in)                   :: level
      integer( kind=c_long )                          :: asagi_grid_get_long

      integer :: l !level send to asagi

      if( present( level ) ) then
        l = level
      else
        l = 0
      endif

      asagi_grid_get_long = asagi_grid_get_long_c( grid_id, pos, l )
    end function asagi_grid_get_long
    
    !> @ingroup f_interface
    !!
    !! @see asagi::Grid::getFloat()
    function asagi_grid_get_float( grid_id, pos, level )
      use, intrinsic :: iso_c_binding
      integer, intent(in)                             :: grid_id
      real( kind=c_double ), dimension(*), intent(in) :: pos
      integer, optional, intent(in)                   :: level
      real                                            :: asagi_grid_get_float

      integer :: l !level send to asagi

      if( present( level ) ) then
        l = level
      else
        l = 0
      endif

      asagi_grid_get_float = asagi_grid_get_float_c( grid_id, pos, l )
    end function asagi_grid_get_float
    
    !> @ingroup f_interface
    !!
    !! @see asagi::Grid::getDouble()
    function asagi_grid_get_double( grid_id, pos, level )
      use, intrinsic :: iso_c_binding
      integer, intent(in)                             :: grid_id
      real( kind=c_double ), dimension(*), intent(in) :: pos
      integer, optional, intent(in)                   :: level
      real( kind=c_double )                           :: asagi_grid_get_double

      integer :: l !level send to asagi

      if( present( level ) ) then
        l = level
      else
        l = 0
      endif

      asagi_grid_get_double = asagi_grid_get_double_c( grid_id, pos, l )
    end function asagi_grid_get_double
    
    !> @ingroup f_interface
    !!
    !! @see asagi::Grid::getBuf()
    subroutine asagi_grid_get_buf( grid_id, buf, pos, level )
      use, intrinsic :: iso_c_binding
      integer, intent(in)                             :: grid_id
      type( c_ptr )                                   :: buf
      real( kind=c_double ), dimension(*), intent(in) :: pos
      integer, optional, intent(in)                   :: level

      integer :: l !level send to asagi

      if( present( level ) ) then
        l = level
      else
        l = 0
      endif

      call asagi_grid_get_buf_c( grid_id, buf, pos, l )
    end subroutine asagi_grid_get_buf
end module asagi
