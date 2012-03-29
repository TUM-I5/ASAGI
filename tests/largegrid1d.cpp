#include <asagi.h>
#include <mpi.h>

#include "debug/dbg.h"

#include "tests.h"

using namespace asagi;

int main(int argc, char** argv)
{
	int rank;
	
	MPI_Init(&argc, &argv);
	
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	
	Grid* grid = Grid::create(Grid::FLOAT, LARGE_GRID);
	
	if (grid->open(NC_1D) != Grid::SUCCESS)
		return 1;
	
	for (int i = 0; i < NC_WIDTH; i++) 
		if (grid->getInt1D(i) != i) {
			dbgDebug() << "Test failed on rank" << rank;
			dbgDebug() << "Value at" << i << "should be"
				<< i << "but is" << grid->getInt1D(i);
			return 1;
		}
	
	delete grid;
	
	MPI_Finalize();
	
	return 0;
}