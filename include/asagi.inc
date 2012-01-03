module asagi
  implicit none

  enum, bind( c )
    enumerator :: GRID_BYTE, GRID_INT, GRID_LONG, GRID_FLOAT, GRID_DOUBLE, &
      GRID_BYTEARRAY, GRID_INTARRAY, GRID_LONGARRAY, GRID_FLOATARRAY, &
      GRID_DOUBLEARRAY
  end enum
  
  interface
    function grid_create( grid_type ) bind( c, name="f95grid_create" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value :: grid_type
      integer( kind=c_int )        :: grid_create
    end function grid_create
    
    function grid_open( grid_id, filename ) bind( c, name="f95grid_open" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value           :: grid_id
      character( kind=c_char ), dimension(*) :: filename
      logical( kind=c_bool )                 :: grid_open
    end function grid_open
    
    function grid_min_x( grid_id ) bind( c, name="f95grid_min_x" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value :: grid_id
      real( kind=c_float )         :: grid_min_x
    end function grid_min_x
    
    function grid_max_x( grid_id ) bind( c, name="f95grid_max_x" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value :: grid_id
      real( kind=c_float )         :: grid_max_x
    end function grid_max_x
    
    function grid_min_y( grid_id ) bind( c, name="f95grid_min_y" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value :: grid_id
      real( kind=c_float )         :: grid_min_y
    end function grid_min_y
    
    function grid_max_y( grid_id ) bind( c, name="f95grid_max_y" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value :: grid_id
      real( kind=c_float )         :: grid_max_y
    end function grid_max_y
    
    function grid_get_byte( grid_id, x, y ) bind( c, name="f95grid_get_byte" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value :: grid_id
      real( kind=c_float ), value  :: x
      real( kind=c_float ), value  :: y
      character( kind=c_char )     :: grid_get_byte
    end function grid_get_byte
    
    function grid_get_int( grid_id, x, y ) bind( c, name="f95grid_get_int" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value :: grid_id
      real( kind=c_float ), value  :: x
      real( kind=c_float ), value  :: y
      integer( kind=c_int )        :: grid_get_int
    end function grid_get_int
    
    function grid_get_long( grid_id, x, y ) bind( c, name="f95grid_get_long" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value :: grid_id
      real( kind=c_float ), value  :: x
      real( kind=c_float ), value  :: y
      integer( kind=c_long )       :: grid_get_long
    end function grid_get_long
    
    function grid_get_float( grid_id, x, y ) bind( c, name="f95grid_get_float" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value :: grid_id
      real( kind=c_float ), value  :: x
      real( kind=c_float ), value  :: y
      real( kind=c_float )         :: grid_get_float
    end function grid_get_float
    
    function grid_get_double( grid_id, x, y ) bind( c, name="f95grid_get_double" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value :: grid_id
      real( kind=c_float ), value  :: x
      real( kind=c_float ), value  :: y
      real( kind=c_double )         :: grid_get_double
    end function grid_get_double
    
    subroutine grid_get_buf( grid_id, x, y, buf ) bind( c, name="f95grid_get_buf" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value :: grid_id
      real( kind=c_float ), value  :: x
      real( kind=c_float ), value  :: y
      type( c_ptr ), value         :: buf
    end subroutine grid_get_buf
    
    subroutine grid_free( grid_id ) bind( c, name="f95grid_free" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value :: grid_id
    end subroutine grid_free

  end interface
end module asagi