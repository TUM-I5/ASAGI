#include <mpi.h>
#include <asagi.h>
#include <stdio.h>

int main(int argc, char** argv)
{
  MPI_Init(&argc, &argv);
  
  asagi_grid* grid = asagi_grid_create(ASAGI_FLOAT);
  asagi_grid_set_comm(grid, MPI_COMM_WORLD);
  // with threads, set number of threads
  asagi_grid_set_threads(grid, 1);
  
  if (asagi_grid_open(grid, "/path/to/netcdf/file.nc", 0) != ASAGI_SUCCESS) {
    printf("Could not load file\n");
    return 1;
  }
  
  double pos[] = {0, 0};
  printf("Value at (0,0): %f\n", asagi_grid_get_float(grid, pos, 0));
  
  asagi_grid_close(grid);
  
  MPI_Finalize();
  
  return 0;
}
