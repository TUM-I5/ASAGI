module database
  implicit none
  
  interface
    function grid_load( filename ) bind( c, name="f95grid_load" )
      use, intrinsic :: iso_c_binding
      character( kind=c_char ), dimension(*) :: filename
      integer( kind=c_int )                  :: grid_load
    end function grid_load
  end interface
end module database