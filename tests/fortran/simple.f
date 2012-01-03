! Simple fortran test

      program simple
        character*256 filename
        integer grid
        real minimum, maximum
        real xpos, ypos
        real value

        filename = '../data/tohoku_1850m_bath.nc'

        call fgrid_load(filename, grid)
        if (grid < 0) then
          write (*,*) 'Could not load file'
          call exit(1)
        end if

        write (*,*) 'File loaded'

        call fgrid_min_x(grid, minimum)
        call fgrid_max_x(grid, maximum)
        write (*,*) 'Range x:', minimum, maximum
        call fgrid_min_y(grid, minimum)
        call fgrid_max_y(grid, maximum)
        write (*,*) 'Range y:', minimum, maximum
        xpos = 5
        ypos = 10.1
        call fgrid_get_value(grid, xpos, ypos, value)
        write (*,*) '5/10.1:', value

      end