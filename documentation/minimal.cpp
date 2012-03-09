#include <mpi.h>
#include <asagi.h>
#include <iostream>

using namespace asagi;

int main(int argc, char** argv)
{
  MPI_Init(&argc, &argv);
  
  Grid* grid = Grid::create();
  
  if (grid->open("/path/to/netcdf/file.nc") != Grid::SUCCESS) {
    std::cout << "Could not load file" << std::endl;
    return 1;
  }
  
  std::cout << "Value at (0,0): " << grid->getFloat2D(0, 0) << std::endl;
  
  delete grid;
  
  MPI_Finalize();
  
  return 0;
}