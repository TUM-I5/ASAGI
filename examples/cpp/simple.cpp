#include <asagi.h>
#include <mpi.h>
#include <stdio.h>
#include <cassert>

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
	
	Grid* grid = Grid::create(Grid::FLOAT);
	grid->setComm(MPI_COMM_WORLD);
	
	if (grid->open("tests/2dgrid.nc") != Grid::SUCCESS) {
		printf("Could not load file\n");
		return 1;
	}
	
	if (rank == 0) {
		printf("Range X: %f - %f\n", grid->getMin(0), grid->getMax(0));
		printf("Range Y: %f - %f\n", grid->getMin(1), grid->getMax(1));
	}
	
	if (rank == 0) {
		//printf("%f\n", grid->getFloat2D(1100000, 0));
		//printf("%f\n", grid->getFloat2D(-250000, 0));
		double pos0[] = {5, 10};
		printf("Value at 5x10: %f\n", grid->getFloat(pos0));
		printf("Double value at 5x10: %f\n", grid->getDouble(pos0));
		printf("Int value at 5x10: %d\n", grid->getInt(pos0));
	} else {
		double pos1[] = {5, 10.1};
		printf("Value at 5x10.1: %f\n", grid->getFloat(pos1));
		double pos2[] = {-1, -15.32};
		printf("Value at -1x-15.32: %f\n", grid->getFloat(pos2));
	}
	
	delete grid;
	
	MPI_Finalize();
	
	return 0;
}
