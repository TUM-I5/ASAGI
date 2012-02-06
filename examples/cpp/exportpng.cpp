#include <asagi.h>
#include <stdio.h>

using namespace asagi;

int main (int argc, char **argv)
{
	int rank;
	Grid* grid;
	
	MPI_Init(&argc, &argv);
	
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	
	Grid::init(MPI_COMM_WORLD);
	
	grid = Grid::create(Grid::FLOAT);
	
	if (!grid->open("../data/noise.nc")) {
		printf("Could not load file\n");
		return 1;
	}
	
	if (rank == 0) {
		if (!grid->exportPng("../data/noice.png")) {
			printf("Could not write png file\n");
			return 1;
		}
	}
	
	delete grid;
	
	grid = Grid::create();
	
	if (!grid->open("../data/tohoku_1850m_bath.nc")) {
	//if (!grid->open("../data/noise.nc")) {
		printf("Could not load file\n");
		return 1;
	}
	
	if (rank == 0) {
		if (!grid->exportPng("../data/bath_0.png")) {
			printf("Could not write png file\n");
			return 1;
		}
	} else {
		if (!grid->exportPng("../data/bath_1.png")) {
			printf("Could not write png file\n");
			return 1;
		}
	}
	
	delete grid;
	
	printf("Png file written\n");
	
	Grid::finalize();
	
	MPI_Finalize();
	printf("Png export not supported\n");
	
	return 0;
}