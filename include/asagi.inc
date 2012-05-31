!>
!! @file
!!  This file is part of ASAGI.
!! 
!!  ASAGI is free software: you can redistribute it and/or modify
!!  it under the terms of the GNU General Public License as published by
!!  the Free Software Foundation, either version 3 of the License, or
!!  (at your option) any later version.
!!
!!  ASAGI is distributed in the hope that it will be useful,
!!  but WITHOUT ANY WARRANTY; without even the implied warranty of
!!  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
!!  GNU General Public License for more details.
!!
!!  You should have received a copy of the GNU General Public License
!!  along with ASAGI.  If not, see <http://www.gnu.org/licenses/>.
!!
!!  Diese Datei ist Teil von ASAGI.
!!
!!  ASAGI ist Freie Software: Sie koennen es unter den Bedingungen
!!  der GNU General Public License, wie von der Free Software Foundation,
!!  Version 3 der Lizenz oder (nach Ihrer Option) jeder spaeteren
!!  veroeffentlichten Version, weiterverbreiten und/oder modifizieren.
!!
!!  ASAGI wird in der Hoffnung, dass es nuetzlich sein wird, aber
!!  OHNE JEDE GEWAEHELEISTUNG, bereitgestellt; sogar ohne die implizite
!!  Gewaehrleistung der MARKTFAEHIGKEIT oder EIGNUNG FUER EINEN BESTIMMTEN
!!  ZWECK. Siehe die GNU General Public License fuer weitere Details.
!!
!!  Sie sollten eine Kopie der GNU General Public License zusammen mit diesem
!!  Programm erhalten haben. Wenn nicht, siehe <http://www.gnu.org/licenses/>.
!! 
!! @copyright 2012 Sebastian Rettenberger <rettenbs@in.tum.de>
!! @version \$Id$
!!
!! @brief Include file for Fortran API
!!
!! @defgroup f_interface Fortran Interface

!> @ingroup f_interface
!!
!! ASAGI Fortran Interface
module asagi
  implicit none

  !> @ingroup f_interface
  !!
  !! @see asagi::Grid::NO_HINT
  integer, parameter :: GRID_NO_HINT = 0
  !> @ingroup f_interface
  !!
  !! @see asagi::Grid::HAS_TIME
  integer, parameter :: GRID_HAS_TIME = 1
  !> @ingroup f_interface
  !!
  !! @see asagi::Grid::LARGE_GRID
  integer, parameter :: GRID_LARGE_GRID = 2
  !> @ingroup f_interface
  !!
  !! @see asagi::Grid::ADAPTIVE
  integer, parameter :: GRID_ADAPTIVE = 4

  !> @ingroup f_interface
  !!
  !! @see asagi::Grid::Type
  enum, bind( c )
    enumerator :: GRID_BYTE, GRID_INT, GRID_LONG, GRID_FLOAT, GRID_DOUBLE
  end enum
  
  !> @ingroup f_interface
  !!
  !! @see asagi::Grid::Error
  enum, bind( c )
    enumerator :: GRID_SUCCESS = 0, GRID_MPI_ERROR, GRID_UNKNOWN_PARAM, &
      GRID_INVALID_VALUE, GRID_NOT_OPEN, GRID_VAR_NOT_FOUND, &
      GRID_UNSUPPORTED_DIMENSIONS, GRID_MULTIPLE_TOPGRIDS, &
      GRID_INVALID_VAR_SIZE
  end enum
  
  !> @cond ingore
  interface
  !> @endcond ingore
  
    !> @internal
    function grid_create_c( grid_type, hint, levels ) bind( c, name="f90grid_create" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value :: grid_type
      integer( kind=c_int ), value :: hint
      integer( kind=c_int ), value :: levels
      integer( kind=c_int )        :: grid_create_c
    end function grid_create_c

    !> @internal
    function grid_create_array_c( grid_basictype, hint, levels ) bind( c, name="f90grid_create_array" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value :: grid_basictype
      integer( kind=c_int ), value :: hint
      integer( kind=c_int ), value :: levels
      integer( kind=c_int )        :: grid_create_array_c
    end function grid_create_array_c

    !> @ingroup f_interface
    !!
    !! @see asagi::Grid::setComm()
    function grid_set_comm( grid_id, comm ) bind( c, name="f90grid_set_comm" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value :: grid_id
      integer( kind=c_int ), value :: comm
      integer( kind=c_int )        :: grid_set_comm
    end function grid_set_comm

    !> @internal
    function grid_set_param_c( grid_id, name, value, level ) bind( c, name="f90grid_set_param" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value                       :: grid_id
      character( kind=c_char ), dimension(*), intent(in) :: name
      character( kind=c_char ), dimension(*), intent(in) :: value
      integer( kind=c_int ), value                       :: level
      integer( kind=c_int )                              :: grid_set_param_c
    end function grid_set_param_c
    
    !> @internal
    function grid_open_c( grid_id, filename, level ) bind( c, name="f90grid_open" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value                       :: grid_id
      character( kind=c_char ), dimension(*), intent(in) :: filename
      integer( kind=c_int ), value                       :: level
      integer( kind=c_int )                              :: grid_open_c
    end function grid_open_c
    
    !> @ingroup f_interface
    !!
    !! @see asagi::Grid::getXMin()
    function grid_min_x( grid_id ) bind( c, name="f90grid_min_x" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value :: grid_id
      real( kind=c_double )        :: grid_min_x
    end function grid_min_x
    
    !> @ingroup f_interface
    !!
    !! @see asagi::Grid::getXMax()
    function grid_max_x( grid_id ) bind( c, name="f90grid_max_x" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value :: grid_id
      real( kind=c_double )        :: grid_max_x
    end function grid_max_x
    
    !> @ingroup f_interface
    !!
    !! @see asagi::Grid::getYMin()
    function grid_min_y( grid_id ) bind( c, name="f90grid_min_y" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value :: grid_id
      real( kind=c_double )        :: grid_min_y
    end function grid_min_y
    
    !> @ingroup f_interface
    !!
    !! @see asagi::Grid::getYMax()
    function grid_max_y( grid_id ) bind( c, name="f90grid_max_y" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value :: grid_id
      real( kind=c_double )        :: grid_max_y
    end function grid_max_y
    
    !> @ingroup f_interface
    !!
    !! @see asagi::Grid::getZMin()
    function grid_min_z( grid_id ) bind( c, name="f90grid_min_z" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value :: grid_id
      real( kind=c_double )        :: grid_min_z
    end function grid_min_z
    
    !> @ingroup f_interface
    !!
    !! @see asagi::Grid::getZMax()
    function grid_max_z( grid_id ) bind( c, name="f90grid_max_z" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value :: grid_id
      real( kind=c_double )        :: grid_max_z
    end function grid_max_z
    
    !> @internal
    function grid_get_byte_1d_c( grid_id, x, level ) bind( c, name="f90grid_get_byte_1d" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value :: grid_id
      real( kind=c_double ), value :: x
      integer( kind=c_int ), value :: level
      character( kind=c_char )     :: grid_get_byte_1d_c
    end function grid_get_byte_1d_c
    
    !> @internal
    function grid_get_int_1d_c( grid_id, x, level ) bind( c, name="f90grid_get_int_1d" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value :: grid_id
      real( kind=c_double ), value :: x
      integer( kind=c_int ), value :: level
      integer( kind=c_int )        :: grid_get_int_1d_c
    end function grid_get_int_1d_c
    
    !> @internal
    function grid_get_long_1d_c( grid_id, x, level ) bind( c, name="f90grid_get_long_1d" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value :: grid_id
      real( kind=c_double ), value :: x
      integer( kind=c_int ), value :: level
      integer( kind=c_long )       :: grid_get_long_1d_c
    end function grid_get_long_1d_c
    
    !> @internal
    function grid_get_float_1d_c( grid_id, x, level ) bind( c, name="f90grid_get_float_1d" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value :: grid_id
      real( kind=c_double ), value :: x
      integer( kind=c_int ), value :: level
      real( kind=c_float )         :: grid_get_float_1d_c
    end function grid_get_float_1d_c
    
    !> @internal
    function grid_get_double_1d_c( grid_id, x, level ) bind( c, name="f90grid_get_double_1d" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value :: grid_id
      real( kind=c_double ), value :: x
      integer( kind=c_int ), value :: level
      real( kind=c_double )        :: grid_get_double_1d_c
    end function grid_get_double_1d_c
    
    !> @internal
    subroutine grid_get_buf_1d_c( grid_id, buf, x, level ) bind( c, name="f90grid_get_buf_1d" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value :: grid_id
      type( c_ptr ), value         :: buf
      real( kind=c_double ), value :: x
      integer( kind=c_int ), value :: level
    end subroutine grid_get_buf_1d_c
    
    !> @internal
    function grid_get_byte_2d_c( grid_id, x, y, level ) bind( c, name="f90grid_get_byte_2d" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value :: grid_id
      real( kind=c_double ), value :: x
      real( kind=c_double ), value :: y
      integer( kind=c_int ), value :: level
      character( kind=c_char )     :: grid_get_byte_2d_c
    end function grid_get_byte_2d_c
    
    !> @internal
    function grid_get_int_2d_c( grid_id, x, y, level ) bind( c, name="f90grid_get_int_2d" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value :: grid_id
      real( kind=c_double ), value :: x
      real( kind=c_double ), value :: y
      integer( kind=c_int ), value :: level
      integer( kind=c_int )        :: grid_get_int_2d_c
    end function grid_get_int_2d_c
    
    !> @internal
    function grid_get_long_2d_c( grid_id, x, y, level ) bind( c, name="f90grid_get_long_2d" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value :: grid_id
      real( kind=c_double ), value :: x
      real( kind=c_double ), value :: y
      integer( kind=c_int ), value :: level
      integer( kind=c_long )       :: grid_get_long_2d_c
    end function grid_get_long_2d_c
    
    !> @internal
    function grid_get_float_2d_c( grid_id, x, y, level ) bind( c, name="f90grid_get_float_2d" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value :: grid_id
      real( kind=c_double ), value :: x
      real( kind=c_double ), value :: y
      integer( kind=c_int ), value :: level
      real( kind=c_float )         :: grid_get_float_2d_c
    end function grid_get_float_2d_c
    
    !> @internal
    function grid_get_double_2d_c( grid_id, x, y, level ) bind( c, name="f90grid_get_double_2d" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value :: grid_id
      real( kind=c_double ), value :: x
      real( kind=c_double ), value :: y
      integer( kind=c_int ), value :: level
      real( kind=c_double )        :: grid_get_double_2d_c
    end function grid_get_double_2d_c
    
    !> @internal
    subroutine grid_get_buf_2d_c( grid_id, buf, x, y, level ) bind( c, name="f90grid_get_buf_2d" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value :: grid_id
      type( c_ptr ), value         :: buf
      real( kind=c_double ), value :: x
      real( kind=c_double ), value :: y
      integer( kind=c_int ), value :: level
    end subroutine grid_get_buf_2d_c
    
    !> @internal
    function grid_get_byte_3d_c( grid_id, x, y, z, level ) bind( c, name="f90grid_get_byte_3d" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value :: grid_id
      real( kind=c_double ), value :: x
      real( kind=c_double ), value :: y
      real( kind=c_double ), value :: z
      integer( kind=c_int ), value :: level
      character( kind=c_char )     :: grid_get_byte_3d_c
    end function grid_get_byte_3d_c
    
    !> @internal
    function grid_get_int_3d_c( grid_id, x, y, z, level ) bind( c, name="f90grid_get_int_3d" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value :: grid_id
      real( kind=c_double ), value :: x
      real( kind=c_double ), value :: y
      real( kind=c_double ), value :: z
      integer( kind=c_int ), value :: level
      integer( kind=c_int )        :: grid_get_int_3d_c
    end function grid_get_int_3d_c
    
    !> @internal
    function grid_get_long_3d_c( grid_id, x, y, z, level ) bind( c, name="f90grid_get_long_3d" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value :: grid_id
      real( kind=c_double ), value :: x
      real( kind=c_double ), value :: y
      real( kind=c_double ), value :: z
      integer( kind=c_int ), value :: level
      integer( kind=c_long )       :: grid_get_long_3d_c
    end function grid_get_long_3d_c
    
    !> @internal
    function grid_get_float_3d_c( grid_id, x, y, z, level ) bind( c, name="f90grid_get_float_3d" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value :: grid_id
      real( kind=c_double ), value :: x
      real( kind=c_double ), value :: y
      real( kind=c_double ), value :: z
      integer( kind=c_int ), value :: level
      real( kind=c_float )         :: grid_get_float_3d_c
    end function grid_get_float_3d_c
    
    !> @internal
    function grid_get_double_3d_c( grid_id, x, y, z, level ) bind( c, name="f90grid_get_double_3d" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value :: grid_id
      real( kind=c_double ), value :: x
      real( kind=c_double ), value :: y
      real( kind=c_double ), value :: z
      integer( kind=c_int ), value :: level
      real( kind=c_double )        :: grid_get_double_3d_c
    end function grid_get_double_3d_c
    
    !> @internal
    subroutine grid_get_buf_3d_c( grid_id, buf, x, y, z, level ) bind( c, name="f90grid_get_buf_3d" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value :: grid_id
      type( c_ptr ), value         :: buf
      real( kind=c_double ), value :: x
      real( kind=c_double ), value :: y
      real( kind=c_double ), value :: z
      integer( kind=c_int ), value :: level
    end subroutine grid_get_buf_3d_c
    
    !> @ingroup f_interface
    !!
    !! @see asagi::Grid#close(asagi::Grid*)
    subroutine grid_close( grid_id ) bind( c, name="f90grid_close" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value :: grid_id
    end subroutine grid_close

  !> @cond ignore
  end interface
  !> @endcond ignore

  !> @ingroup f_interface
  !!
  !! Interface for arbitrary dimensions
  interface grid_get_byte
    module procedure grid_get_byte_1d
    module procedure grid_get_byte_2d
    module procedure grid_get_byte_3d
  end interface grid_get_byte

  !> @ingroup f_interface
  !!
  !! Interface for arbitrary dimensions
  interface grid_get_int
    module procedure grid_get_int_1d
    module procedure grid_get_int_2d
    module procedure grid_get_int_3d
  end interface grid_get_int

  !> @ingroup f_interface
  !!
  !! Interface for arbitrary dimensions
  interface grid_get_long
    module procedure grid_get_long_1d
    module procedure grid_get_long_2d
    module procedure grid_get_long_3d
  end interface grid_get_long
  
  !> @ingroup f_interface
  !!
  !! Interface for arbitrary dimensions
  interface grid_get_float
    module procedure grid_get_float_1d
    module procedure grid_get_float_2d
    module procedure grid_get_float_3d
  end interface grid_get_float

  !> @ingroup f_interface
  !!
  !! Interface for arbitrary dimensions
  interface grid_get_double
    module procedure grid_get_double_1d
    module procedure grid_get_double_2d
    module procedure grid_get_double_3d
  end interface grid_get_double

  !> @ingroup f_interface
  !!
  !! Interface for arbitrary dimensions
  interface grid_get_buf
    module procedure grid_get_buf_1d
    module procedure grid_get_buf_2d
    module procedure grid_get_buf_3d
  end interface grid_get_buf

  contains
    !> @ingroup f_interface
    !!
    !! @see asagi::Grid::create()
    function grid_create( grid_type, hint, levels )
      integer, optional, intent(in) :: grid_type
      integer, optional, intent(in) :: hint
      integer, optional, intent(in) :: levels
      integer                       :: grid_create

      !variables send to asagi
      integer :: g, h, l

      if( present( grid_type ) ) then
        g = grid_type
      else
        g = GRID_FLOAT
      endif
      if( present( hint ) ) then
        h = hint
      else
        h = GRID_NO_HINT
      endif
      if( present( levels ) ) then
        l = levels
      else
        l = 1
      endif

      grid_create = grid_create_c( g, h, l )
    end function grid_create

    !> @ingroup f_interface
    !!
    !! @see asagi::Grid::createArray()
    function grid_create_array( grid_basictype, hint, levels )
      integer, optional, intent(in) :: grid_basictype
      integer, optional, intent(in) :: hint
      integer, optional, intent(in) :: levels
      integer                       :: grid_create_array

      !variables send to asagi
      integer :: g, h, l

      if( present( grid_basictype ) ) then
        g = grid_basictype
      else
        g = GRID_FLOAT
      endif
      if( present( hint ) ) then
        h = hint
      else
        h = GRID_NO_HINT
      endif
      if( present( levels ) ) then
        l = levels
      else
        l = 1
      endif

      grid_create_array = grid_create_array_c( g, h, l )
    end function grid_create_array

    !> @ingroup f_interface
    !!
    !! @see asagi::Grid::setParam()
    function grid_set_param( grid_id, name, value, level )
      use, intrinsic :: iso_c_binding
      integer, intent(in)           :: grid_id
      character*(*), intent(in)     :: name
      character*(*), intent(in)     :: value
      integer, optional, intent(in) :: level
      integer                       :: grid_set_param

      integer :: l !level send to asagi

      if( present( level ) ) then
        l = level
      else
        l = 0
      endif

      grid_set_param = grid_set_param_c( grid_id, name // c_null_char, &
        value // c_null_char, l )
    end function grid_set_param

    !> @ingroup f_interface
    !!
    !! @see asagi::Grid::open()
    function grid_open( grid_id, filename, level )
      use, intrinsic :: iso_c_binding
      integer, intent(in)           :: grid_id
      character*(*), intent(in)     :: filename
      integer, optional, intent(in) :: level
      integer                       :: grid_open

      integer :: l !level send to asagi

      if( present( level ) ) then
        l = level
      else
        l = 0
      endif

      grid_open = grid_open_c( grid_id, filename // c_null_char, l )
    end function grid_open
    
    !> @ingroup f_interface
    !!
    !! @see asagi::Grid::getByte1D()
    function grid_get_byte_1d( grid_id, x, level )
      use, intrinsic :: iso_c_binding
      integer, intent(in)               :: grid_id
      real( kind=c_double ), intent(in) :: x
      integer, optional, intent(in)     :: level
      character                         :: grid_get_byte_1d

      integer :: l !level send to asagi

      if( present( level ) ) then
        l = level
      else
        l = 0
      endif

      grid_get_byte_1d = grid_get_byte_1d_c( grid_id, x, l )
    end function grid_get_byte_1d
    
    !> @ingroup f_interface
    !!
    !! @see asagi::Grid::getInt1D()
    function grid_get_int_1d( grid_id, x, level )
      use, intrinsic :: iso_c_binding
      integer, intent(in)               :: grid_id
      real( kind=c_double ), intent(in) :: x
      integer, optional, intent(in)     :: level
      integer                           :: grid_get_int_1d

      integer :: l !level send to asagi

      if( present( level ) ) then
        l = level
      else
        l = 0
      endif

      grid_get_int_1d = grid_get_int_1d_c( grid_id, x, l )
    end function grid_get_int_1d
    
    !> @ingroup f_interface
    !!
    !! @see asagi::Grid::getLong1D()
    function grid_get_long_1d( grid_id, x, level )
      use, intrinsic :: iso_c_binding
      integer, intent(in)               :: grid_id
      real( kind=c_double ), intent(in) :: x
      integer, optional, intent(in)     :: level
      integer( kind=c_long )            :: grid_get_long_1d

      integer :: l !level send to asagi

      if( present( level ) ) then
        l = level
      else
        l = 0
      endif

      grid_get_long_1d = grid_get_long_1d_c( grid_id, x, l )
    end function grid_get_long_1d
    
    !> @ingroup f_interface
    !!
    !! @see asagi::Grid::getFloat1D()
    function grid_get_float_1d( grid_id, x, level )
      use, intrinsic :: iso_c_binding
      integer, intent(in)               :: grid_id
      real( kind=c_double ), intent(in) :: x
      integer, optional, intent(in)     :: level
      real                              :: grid_get_float_1d

      integer :: l !level send to asagi

      if( present( level ) ) then
        l = level
      else
        l = 0
      endif

      grid_get_float_1d = grid_get_float_1d_c( grid_id, x, l )
    end function grid_get_float_1d
    
    !> @ingroup f_interface
    !!
    !! @see asagi::Grid::getDouble1D()
    function grid_get_double_1d( grid_id, x, level )
      use, intrinsic :: iso_c_binding
      integer, intent(in)               :: grid_id
      real( kind=c_double ), intent(in) :: x
      integer, optional, intent(in)     :: level
      real( kind=c_double )             :: grid_get_double_1d

      integer :: l !level send to asagi

      if( present( level ) ) then
        l = level
      else
        l = 0
      endif

      grid_get_double_1d = grid_get_double_1d_c( grid_id, x, l )
    end function grid_get_double_1d
    
    !> @ingroup f_interface
    !!
    !! @see asagi::Grid::getBuf1D()
    subroutine grid_get_buf_1d( grid_id, buf, x, level )
      use, intrinsic :: iso_c_binding
      integer, intent(in)               :: grid_id
      type( c_ptr )                     :: buf
      real( kind=c_double ), intent(in) :: x
      integer, optional, intent(in)     :: level

      integer :: l !level send to asagi

      if( present( level ) ) then
        l = level
      else
        l = 0
      endif

      call grid_get_buf_1d_c( grid_id, buf, x, l )
    end subroutine grid_get_buf_1d
    
    !> @ingroup f_interface
    !!
    !! @see asagi::Grid::getByte2D()
    function grid_get_byte_2d( grid_id, x, y, level )
      use, intrinsic :: iso_c_binding
      integer, intent(in)               :: grid_id
      real( kind=c_double ), intent(in) :: x
      real( kind=c_double ), intent(in) :: y
      integer, optional, intent(in)     :: level
      character                         :: grid_get_byte_2d

      integer :: l !level send to asagi

      if( present( level ) ) then
        l = level
      else
        l = 0
      endif

      grid_get_byte_2d = grid_get_byte_2d_c( grid_id, x, y, l )
    end function grid_get_byte_2d
    
    !> @ingroup f_interface
    !!
    !! @see asagi::Grid::getInt2D()
    function grid_get_int_2d( grid_id, x, y, level )
      use, intrinsic :: iso_c_binding
      integer, intent(in)               :: grid_id
      real( kind=c_double ), intent(in) :: x
      real( kind=c_double ), intent(in) :: y
      integer, optional, intent(in)     :: level
      integer                           :: grid_get_int_2d

      integer :: l !level send to asagi

      if( present( level ) ) then
        l = level
      else
        l = 0
      endif

      grid_get_int_2d = grid_get_int_2d_c( grid_id, x, y, l )
    end function grid_get_int_2d
    
    !> @ingroup f_interface
    !!
    !! @see asagi::Grid::getLong2D()
    function grid_get_long_2d( grid_id, x, y, level )
      use, intrinsic :: iso_c_binding
      integer, intent(in)               :: grid_id
      real( kind=c_double ), intent(in) :: x
      real( kind=c_double ), intent(in) :: y
      integer, optional, intent(in)     :: level
      integer( kind=c_long )            :: grid_get_long_2d

      integer :: l !level send to asagi

      if( present( level ) ) then
        l = level
      else
        l = 0
      endif

      grid_get_long_2d = grid_get_long_2d_c( grid_id, x, y, l )
    end function grid_get_long_2d
    
    !> @ingroup f_interface
    !!
    !! @see asagi::Grid::getFloat2D()
    function grid_get_float_2d( grid_id, x, y, level )
      use, intrinsic :: iso_c_binding
      integer, intent(in)               :: grid_id
      real( kind=c_double ), intent(in) :: x
      real( kind=c_double ), intent(in) :: y
      integer, optional, intent(in)     :: level
      real                              :: grid_get_float_2d

      integer :: l !level send to asagi

      if( present( level ) ) then
        l = level
      else
        l = 0
      endif

      grid_get_float_2d = grid_get_float_2d_c( grid_id, x, y, l )
    end function grid_get_float_2d
    
    !> @ingroup f_interface
    !!
    !! @see asagi::Grid::getDouble2D()
    function grid_get_double_2d( grid_id, x, y, level )
      use, intrinsic :: iso_c_binding
      integer, intent(in)               :: grid_id
      real( kind=c_double ), intent(in) :: x
      real( kind=c_double ), intent(in) :: y
      integer, optional, intent(in)     :: level
      real( kind=c_double )             :: grid_get_double_2d

      integer :: l !level send to asagi

      if( present( level ) ) then
        l = level
      else
        l = 0
      endif

      grid_get_double_2d = grid_get_double_2d_c( grid_id, x, y, l )
    end function grid_get_double_2d
    
    !> @ingroup f_interface
    !!
    !! @see asagi::Grid::getBuf2D()
    subroutine grid_get_buf_2d( grid_id, buf, x, y, level )
      use, intrinsic :: iso_c_binding
      integer, intent(in)               :: grid_id
      type( c_ptr )                     :: buf
      real( kind=c_double ), intent(in) :: x
      real( kind=c_double ), intent(in) :: y
      integer, optional, intent(in)     :: level

      integer :: l !level send to asagi

      if( present( level ) ) then
        l = level
      else
        l = 0
      endif

      call grid_get_buf_2d_c( grid_id, buf, x, y, l )
    end subroutine grid_get_buf_2d
    
    !> @ingroup f_interface
    !!
    !! @see asagi::Grid::getByte3D()
    function grid_get_byte_3d( grid_id, x, y, z, level )
      use, intrinsic :: iso_c_binding
      integer, intent(in)               :: grid_id
      real( kind=c_double ), intent(in) :: x
      real( kind=c_double ), intent(in) :: y
      real( kind=c_double ), intent(in) :: z
      integer, optional, intent(in)     :: level
      character                         :: grid_get_byte_3d

      integer :: l !level send to asagi

      if( present( level ) ) then
        l = level
      else
        l = 0
      endif

      grid_get_byte_3d = grid_get_byte_3d_c( grid_id, x, y, z, l )
    end function grid_get_byte_3d
    
    !> @ingroup f_interface
    !!
    !! @see asagi::Grid::getInt3D()
    function grid_get_int_3d( grid_id, x, y, z, level )
      use, intrinsic :: iso_c_binding
      integer, intent(in)               :: grid_id
      real( kind=c_double ), intent(in) :: x
      real( kind=c_double ), intent(in) :: y
      real( kind=c_double ), intent(in) :: z
      integer, optional, intent(in)     :: level
      integer                           :: grid_get_int_3d

      integer :: l !level send to asagi

      if( present( level ) ) then
        l = level
      else
        l = 0
      endif

      grid_get_int_3d = grid_get_int_3d_c( grid_id, x, y, z, l )
    end function grid_get_int_3d
    
    !> @ingroup f_interface
    !!
    !! @see asagi::Grid::getLong3D()
    function grid_get_long_3d( grid_id, x, y, z, level )
      use, intrinsic :: iso_c_binding
      integer, intent(in)               :: grid_id
      real( kind=c_double ), intent(in) :: x
      real( kind=c_double ), intent(in) :: y
      real( kind=c_double ), intent(in) :: z
      integer, optional, intent(in)     :: level
      integer( kind=c_long )            :: grid_get_long_3d

      integer :: l !level send to asagi

      if( present( level ) ) then
        l = level
      else
        l = 0
      endif

      grid_get_long_3d = grid_get_long_3d_c( grid_id, x, y, z, l )
    end function grid_get_long_3d
    
    !> @ingroup f_interface
    !!
    !! @see asagi::Grid::getFloat3D()
    function grid_get_float_3d( grid_id, x, y, z, level )
      use, intrinsic :: iso_c_binding
      integer, intent(in)               :: grid_id
      real( kind=c_double ), intent(in) :: x
      real( kind=c_double ), intent(in) :: y
      real( kind=c_double ), intent(in) :: z
      integer, optional, intent(in)     :: level
      real                              :: grid_get_float_3d

      integer :: l !level send to asagi

      if( present( level ) ) then
        l = level
      else
        l = 0
      endif

      grid_get_float_3d = grid_get_float_3d_c( grid_id, x, y, z, l )
    end function grid_get_float_3d
    
    !> @ingroup f_interface
    !!
    !! @see asagi::Grid::getDouble3D()
    function grid_get_double_3d( grid_id, x, y, z, level )
      use, intrinsic :: iso_c_binding
      integer, intent(in)               :: grid_id
      real( kind=c_double ), intent(in) :: x
      real( kind=c_double ), intent(in) :: y
      real( kind=c_double ), intent(in) :: z
      integer, optional, intent(in)     :: level
      real( kind=c_double )             :: grid_get_double_3d

      integer :: l !level send to asagi

      if( present( level ) ) then
        l = level
      else
        l = 0
      endif

      grid_get_double_3d = grid_get_double_3d_c( grid_id, x, y, z, l )
    end function grid_get_double_3d
    
    !> @ingroup f_interface
    !!
    !! @see asagi::Grid::getBuf3D()
    subroutine grid_get_buf_3d( grid_id, buf, x, y, z, level )
      use, intrinsic :: iso_c_binding
      integer, intent(in)               :: grid_id
      type( c_ptr )                     :: buf
      real( kind=c_double ), intent(in) :: x
      real( kind=c_double ), intent(in) :: y
      real( kind=c_double ), intent(in) :: z
      integer, optional, intent(in)     :: level

      integer :: l !level send to asagi

      if( present( level ) ) then
        l = level
      else
        l = 0
      endif

      call grid_get_buf_3d_c( grid_id, buf, x, y, z, l )
    end subroutine grid_get_buf_3d
end module asagi
