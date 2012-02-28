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
	
	Grid* grid = Grid::create(); // FLOAT is default
	
	if (grid->init() != Grid::SUCCESS)
		return 1;
	
	if (grid->open(NC_2D) != Grid::SUCCESS)
		return 1;
	
	int value;
	
	for (int i = 0; i < NC_WIDTH; i++) {
		for (int j = 0; j < NC_LENGTH; j++) {
			value = j * NC_WIDTH + i;
			if (grid->getInt2D(i, j) != value) {
				dbgDebug() << "Test failed on rank" << rank;
				dbgDebug() << "Value at" << i << j << "should be"
					<< value << "but is" << grid->getInt2D(i, j);
				return 1;
			}
		}
	}
	
	delete grid;
	
	MPI_Finalize();
	
	return 0;
}