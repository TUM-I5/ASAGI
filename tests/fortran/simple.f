! Simple fortran test

      program simple
        character*256 filename
        integer grid
        integer x, y
        real value

        filename = '../data/tohoku_1850m_bath.nc'

        call fgrid_load(filename, grid)
        if (grid < 0) then
          write (*,*) 'Could not load file'
          call exit(1)
        end if

        write (*,*) 'File loaded'

        call fgrid_x(grid, x)
        call fgrid_y(grid, y)
        write (*,*) 'x/y dim:', x, y
        call fgrid_get_value(grid, 5, 10, value)
        write (*,*) '5/10:', value

      end