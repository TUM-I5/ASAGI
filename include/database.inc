module database
  implicit none
  
  integer, parameter :: GRID_NULL = -1
  
  interface
    function grid_load( filename ) bind( c, name="f95grid_load" )
      use, intrinsic :: iso_c_binding
      character( kind=c_char ), dimension(*) :: filename
      integer( kind=c_int )                  :: grid_load
    end function grid_load
    
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
    
    function grid_get_value( grid_id, x, y ) bind( c, name="f95grid_get_value" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value :: grid_id
      real( kind=c_float ), value  :: x
      real( kind=c_float ), value  :: y
      real( kind=c_float )         :: grid_get_value
    end function grid_get_value
    
    subroutine grid_free( grid_id ) bind( c, name="f95grid_free" )
      use, intrinsic :: iso_c_binding
      integer( kind=c_int ), value :: grid_id
    end subroutine grid_free

  end interface
end module database