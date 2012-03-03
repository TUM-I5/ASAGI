#include <asagi.h>
#include <mpi.h>
#include <stdio.h>

using namespace asagi;

int main (int argc, char **argv)
{
	int rank, size;
	float value;
	
	MPI_Init(&argc, &argv);
	
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	
	if (rank == 0) {
		MPI_Comm_size(MPI_COMM_WORLD, &size);
		printf("Running on %d nodes\n", size);
	}
	
	// TODO array is currently not supported
	Grid* grid = Grid::createArray(Grid::FLOAT);
	
	if (grid->open("../data/tohoku_1850m_bath.nc") != Grid::SUCCESS) {
		printf("Could not load file\n");
		return 1;
	}
	
	if (rank == 0) {
		printf("Range X: %f-%f\n", grid->getXMin(), grid->getXMax());
		printf("Range Y: %f-%f\n", grid->getYMin(), grid->getYMax());
	
		printf("Value at 5x10: %f\n", grid->getFloat2D(5, 10));
		grid->getBuf2D(&value, 5, 10);
		printf("Value at 5x10: %f\n", value);
	}
	
	delete grid;
	
	MPI_Finalize();
	
	return 0;
}