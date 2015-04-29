#include <mpi.h>
#include <asagi.h>
#include <iostream>

using namespace asagi;

int main(int argc, char** argv)
{
  MPI_Init(&argc, &argv);
  
  Grid* grid = Grid::create();
  grid->setComm(MPI_COMM_WORLD);
  // with threads, set number of threads
  grid->setThreads(1);
  
  if (grid->open("/path/to/netcdf/file.nc") != Grid::SUCCESS) {
    std::cout << "Could not load file" << std::endl;
    return 1;
  }
  
  double pos[] = {0, 0};
  std::cout << "Value at (0,0): " << grid->getFloat(pos) << std::endl;
  
  // The same as: "Grid::close(grid);"
  delete grid;
  
  MPI_Finalize();
  
  return 0;
}
