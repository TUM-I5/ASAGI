#include <asagi.h>
#include <stdio.h>

using namespace asagi;

int main (void)
{
	float value;
	Grid* grid = Grid::create(Grid::FLOATARRAY);
	
	if (!grid->open("../data/tohoku_1850m_bath.nc")) {
		printf("Could not load file\n");
		return 1;
	}
	
	printf("Range X: %f-%f\n", grid->getXMin(), grid->getXMax());
	printf("Range Y: %f-%f\n", grid->getYMin(), grid->getYMax());
	
	printf("Value at 5x10: %f\n", grid->getFloat(5, 10));
	grid->getBuf(5, 10, &value);
	printf("Value at 5x10: %f\n", value);
	
	return 0;
}