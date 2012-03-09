#include <mpi.h>
#include <asagi.h>
#include <stdio.h>

int main(int argc, char** argv)
{
  MPI_Init(&argc, &argv);
  
  grid_handle* grid = grid_create(GRID_FLOAT, GRID_NO_HINT, 1);
  
  if (grid_open(grid, "/path/to/netcdf/file.nc", 0) != GRID_SUCCESS) {
    printf("Could not load file\n");
    return 1;
  }
  
  printf("Value at (0,0): %f\n", grid_get_float_2d(grid, 0, 0, 0));
  
  grid_close(grid);
  
  MPI_Finalize();
  
  return 0;
}