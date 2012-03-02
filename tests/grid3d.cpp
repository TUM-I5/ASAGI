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
	
	if (grid->open(NC_3D) != Grid::SUCCESS)
		return 1;
	
	long value;
	
	for (int i = 0; i < NC_WIDTH; i++) {
		for (int j = 0; j < NC_LENGTH; j++) {
			for (int k = 0; k < NC_HEIGHT; k++) {
				value = (k * NC_LENGTH + j) * NC_WIDTH + i;
				if (grid->getLong3D(i, j, k) != value) {
					dbgDebug() << "Test failed on rank" << rank;
					dbgDebug() << "Value at" << i << j << k << "should be"
						<< value << "but is" << grid->getInt3D(i, j, k);
					return 1;
				}
			}
		}
	}
	
	delete grid;
	
	MPI_Finalize();
	
	return 0;
}