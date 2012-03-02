module asagi
  implicit none

  integer, parameter :: GRID_NO_HINT = 0
  integer, parameter :: GRID_HAS_TIME = 1

  ! grid types
  enum, bind( c )
    enumerator :: GRID_BYTE, GRID_INT, GRID_LONG, GRID_FLOAT, GRID_DOUBLE
  end enum
  
  ! errors
  enum, bind( c )
    enumerator :: GRID_SUCCESS = 0, GRID_MPI_ERROR, GRID_UNKNOWN_PARAM, &
      GRID_INVALID_VALUE, GRID_NOT_OPEN, GRID_VAR_NOT_FOUND, &
      GRID_UNSUPPORTED_DIMENSIONS, GRID_INVALID_VAR_SIZE
  end enum
  
  interface
    ! Init
    function grid_create_c( grid_type, hint, levels ) bind( c, name="f90grid_create" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value :: grid_type
      integer( kind=c_int ), value :: hint
      integer( kind=c_int ), value :: levels
      integer( kind=c_int )        :: grid_create_c
    end function grid_create_c

    function grid_set_comm( grid_id, comm ) bind( c, name="f90grid_set_comm" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value :: grid_id
      integer( kind=c_int ), value :: comm
      integer( kind=c_int )        :: grid_set_comm
    end function grid_set_comm

    function grid_set_param_c( grid_id, name, value, level ) bind( c, name="f90grid_set_param" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value                       :: grid_id
      character( kind=c_char ), dimension(*), intent(in) :: name
      character( kind=c_char ), dimension(*), intent(in) :: value
      integer( kind=c_int ), value                       :: level
      integer( kind=c_int )                              :: grid_set_param_c
    end function grid_set_param_c
    
    function grid_open_c( grid_id, filename, level ) bind( c, name="f90grid_open" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value                       :: grid_id
      character( kind=c_char ), dimension(*), intent(in) :: filename
      integer( kind=c_int ), value                       :: level
      integer( kind=c_int )                              :: grid_open_c
    end function grid_open_c
    
    ! Dimension
    function grid_min_x( grid_id ) bind( c, name="f90grid_min_x" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value :: grid_id
      real( kind=c_double )        :: grid_min_x
    end function grid_min_x
    
    function grid_max_x( grid_id ) bind( c, name="f90grid_max_x" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value :: grid_id
      real( kind=c_double )        :: grid_max_x
    end function grid_max_x
    
    function grid_min_y( grid_id ) bind( c, name="f90grid_min_y" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value :: grid_id
      real( kind=c_double )        :: grid_min_y
    end function grid_min_y
    
    function grid_max_y( grid_id ) bind( c, name="f90grid_max_y" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value :: grid_id
      real( kind=c_double )        :: grid_max_y
    end function grid_max_y
    
    function grid_min_z( grid_id ) bind( c, name="f90grid_min_z" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value :: grid_id
      real( kind=c_double )        :: grid_min_z
    end function grid_min_z
    
    function grid_max_z( grid_id ) bind( c, name="f90grid_max_z" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value :: grid_id
      real( kind=c_double )        :: grid_max_z
    end function grid_max_z
    
    ! 1D
    function grid_get_byte_1d_c( grid_id, x, level ) bind( c, name="f90grid_get_byte_1d" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value :: grid_id
      real( kind=c_double ), value :: x
      integer( kind=c_int ), value :: level
      character( kind=c_char )     :: grid_get_byte_1d_c
    end function grid_get_byte_1d_c
    
    function grid_get_int_1d_c( grid_id, x, level ) bind( c, name="f90grid_get_int_1d" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value :: grid_id
      real( kind=c_double ), value :: x
      integer( kind=c_int ), value :: level
      integer( kind=c_int )        :: grid_get_int_1d_c
    end function grid_get_int_1d_c
    
    function grid_get_long_1d_c( grid_id, x, level ) bind( c, name="f90grid_get_long_1d" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value :: grid_id
      real( kind=c_double ), value :: x
      integer( kind=c_int ), value :: level
      integer( kind=c_long )       :: grid_get_long_1d_c
    end function grid_get_long_1d_c
    
    function grid_get_float_1d_c( grid_id, x, level ) bind( c, name="f90grid_get_float_1d" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value :: grid_id
      real( kind=c_double ), value :: x
      integer( kind=c_int ), value :: level
      real( kind=c_float )         :: grid_get_float_1d_c
    end function grid_get_float_1d_c
    
    function grid_get_double_1d_c( grid_id, x, level ) bind( c, name="f90grid_get_double_1d" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value :: grid_id
      real( kind=c_double ), value :: x
      integer( kind=c_int ), value :: level
      real( kind=c_double )        :: grid_get_double_1d_c
    end function grid_get_double_1d_c
    
    subroutine grid_get_buf_1d_c( grid_id, buf, x, level ) bind( c, name="f90grid_get_buf_1d" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value :: grid_id
      type( c_ptr ), value         :: buf
      real( kind=c_double ), value :: x
      integer( kind=c_int ), value :: level
    end subroutine grid_get_buf_1d_c
    
    ! 2D
    function grid_get_byte_2d_c( grid_id, x, y, level ) bind( c, name="f90grid_get_byte_2d" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value :: grid_id
      real( kind=c_double ), value :: x
      real( kind=c_double ), value :: y
      integer( kind=c_int ), value :: level
      character( kind=c_char )     :: grid_get_byte_2d_c
    end function grid_get_byte_2d_c
    
    function grid_get_int_2d_c( grid_id, x, y, level ) bind( c, name="f90grid_get_int_2d" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value :: grid_id
      real( kind=c_double ), value :: x
      real( kind=c_double ), value :: y
      integer( kind=c_int ), value :: level
      integer( kind=c_int )        :: grid_get_int_2d_c
    end function grid_get_int_2d_c
    
    function grid_get_long_2d_c( grid_id, x, y, level ) bind( c, name="f90grid_get_long_2d" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value :: grid_id
      real( kind=c_double ), value :: x
      real( kind=c_double ), value :: y
      integer( kind=c_int ), value :: level
      integer( kind=c_long )       :: grid_get_long_2d_c
    end function grid_get_long_2d_c
    
    function grid_get_float_2d_c( grid_id, x, y, level ) bind( c, name="f90grid_get_float_2d" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value :: grid_id
      real( kind=c_double ), value :: x
      real( kind=c_double ), value :: y
      integer( kind=c_int ), value :: level
      real( kind=c_float )         :: grid_get_float_2d_c
    end function grid_get_float_2d_c
    
    function grid_get_double_2d_c( grid_id, x, y, level ) bind( c, name="f90grid_get_double_2d" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value :: grid_id
      real( kind=c_double ), value :: x
      real( kind=c_double ), value :: y
      integer( kind=c_int ), value :: level
      real( kind=c_double )        :: grid_get_double_2d_c
    end function grid_get_double_2d_c
    
    subroutine grid_get_buf_2d_c( grid_id, buf, x, y, level ) bind( c, name="f90grid_get_buf_2d" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value :: grid_id
      type( c_ptr ), value         :: buf
      real( kind=c_double ), value :: x
      real( kind=c_double ), value :: y
      integer( kind=c_int ), value :: level
    end subroutine grid_get_buf_2d_c
    
    ! 3D
    function grid_get_byte_3d_c( grid_id, x, y, z, level ) bind( c, name="f90grid_get_byte_3d" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value :: grid_id
      real( kind=c_double ), value :: x
      real( kind=c_double ), value :: y
      real( kind=c_double ), value :: z
      integer( kind=c_int ), value :: level
      character( kind=c_char )     :: grid_get_byte_3d_c
    end function grid_get_byte_3d_c
    
    function grid_get_int_3d_c( grid_id, x, y, z, level ) bind( c, name="f90grid_get_int_3d" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value :: grid_id
      real( kind=c_double ), value :: x
      real( kind=c_double ), value :: y
      real( kind=c_double ), value :: z
      integer( kind=c_int ), value :: level
      integer( kind=c_int )        :: grid_get_int_3d_c
    end function grid_get_int_3d_c
    
    function grid_get_long_3d_c( grid_id, x, y, z, level ) bind( c, name="f90grid_get_long_3d" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value :: grid_id
      real( kind=c_double ), value :: x
      real( kind=c_double ), value :: y
      real( kind=c_double ), value :: z
      integer( kind=c_int ), value :: level
      integer( kind=c_long )       :: grid_get_long_3d_c
    end function grid_get_long_3d_c
    
    function grid_get_float_3d_c( grid_id, x, y, z, level ) bind( c, name="f90grid_get_float_3d" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value :: grid_id
      real( kind=c_double ), value :: x
      real( kind=c_double ), value :: y
      real( kind=c_double ), value :: z
      integer( kind=c_int ), value :: level
      real( kind=c_float )         :: grid_get_float_3d_c
    end function grid_get_float_3d_c
    
    function grid_get_double_3d_c( grid_id, x, y, z, level ) bind( c, name="f90grid_get_double_3d" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value :: grid_id
      real( kind=c_double ), value :: x
      real( kind=c_double ), value :: y
      real( kind=c_double ), value :: z
      integer( kind=c_int ), value :: level
      real( kind=c_double )        :: grid_get_double_3d_c
    end function grid_get_double_3d_c
    
    subroutine grid_get_buf_3d_c( grid_id, buf, x, y, z, level ) bind( c, name="f90grid_get_buf_3d" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value :: grid_id
      type( c_ptr ), value         :: buf
      real( kind=c_double ), value :: x
      real( kind=c_double ), value :: y
      real( kind=c_double ), value :: z
      integer( kind=c_int ), value :: level
    end subroutine grid_get_buf_3d_c
    
    ! Free
    subroutine grid_free( grid_id ) bind( c, name="f90grid_free" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value :: grid_id
    end subroutine grid_free

  end interface

  contains
    ! Init
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
    
    ! 1D
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
    
    ! 2D
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
    
    ! 3D
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
