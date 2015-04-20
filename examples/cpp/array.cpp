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
	
	Grid* grid = Grid::createArray(Grid::FLOAT);
	grid->setComm(MPI_COMM_WORLD);
	
	if (grid->open("tests/2dgrid.nc") != Grid::SUCCESS) {
		printf("Could not load file\n");
		return 1;
	}
	
	if (rank == 0) {
		printf("Range X: %f-%f\n", grid->getMin(0), grid->getMax(0));
		printf("Range Y: %f-%f\n", grid->getMin(1), grid->getMax(1));

		printf("Variable size: %d\n", grid->getVarSize());
	
		double pos[] = {5, 10};
		printf("Value at 5x10: %f\n", grid->getFloat(pos));
		grid->getBuf(&value, pos);
		printf("Value at 5x10: %f\n", value);
	}
	
	delete grid;
	
	MPI_Finalize();
	
	return 0;
}
