#include <asagi.h>
#include <mpi.h>
#include <stdio.h>

using namespace asagi;

int main (int argc, char **argv)
{
	int rank, size;
	
	MPI_Init(&argc, &argv);
	
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	
	if (rank == 0) {
		MPI_Comm_size(MPI_COMM_WORLD, &size);
		printf("Running on %d nodes\n", size);
	}
	
	Grid::init(MPI_COMM_WORLD);
	
	Grid* grid = Grid::create(Grid::FLOAT);
	
	if (!grid->open("../data/tohoku_1850m_bath.nc")) {
		printf("Could not load file\n");
		return 1;
	}
	
	if (rank == 0) {
		printf("Range X: %f-%f\n", grid->getXMin(), grid->getXMax());
		printf("Range Y: %f-%f\n", grid->getYMin(), grid->getYMax());
	}
	
	if (rank == 0)
		printf("Value at 5x10: %f\n", grid->getFloat(5, 10));
	else {
		printf("Value at 5x10.1: %f\n", grid->getFloat(5, 10.1));
		printf("Value at -1x-15.32: %f\n", grid->getFloat(-1, -15.32));
	}
	
	delete grid;
	
	Grid::finalize();
	
	MPI_Finalize();
	
	return 0;
}