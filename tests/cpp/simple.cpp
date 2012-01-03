#include <stdio.h>
#include <database.h>

using namespace grid;

int main (void)
{
	Grid* grid = Grid::create();
	
	if (!grid->open("../data/tohoku_1850m_bath.nc")) {
		printf("Could not load file\n");
		return 1;
	}
	
	printf("Dim (x*y): %u*%u\n", (unsigned int) grid->getXDim(), (unsigned int) grid->getYDim());
	
	printf("Value at 5x10: %f\n", grid->get(5, 10));
	
	return 0;
}