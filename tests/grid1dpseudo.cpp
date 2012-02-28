#include <asagi.h>
#include <mpi.h>
#include <cstdlib>
#include <time.h>

#include "debug/dbg.h"

#include "tests.h"

using namespace asagi;

int main(int argc, char** argv)
{
	int rank;
	
	MPI_Init(&argc, &argv);
	
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	
	Grid* grid = Grid::create(); // FLOAT is default
	
	if (grid->init() != Grid::SUCCESS)
		return 1;
	
	if (grid->open(NC_1DPSEUDO) != Grid::SUCCESS)
		return 1;
	
	srand(time(NULL));
	
	int r;
	
	for (int i = 0; i < NC_WIDTH; i++) {
		r = rand() - (RAND_MAX / 2);
		if (grid->getInt2D(i, r) != i) {
			dbgDebug() << "Test failed on rank" << rank;
			dbgDebug() << "Value at" << i << r << "should be"
				<< i << "but is" << grid->getInt1D(i, r);
			return 1;
		}
	}
	
	delete grid;
	
	MPI_Finalize();
	
	return 0;
}