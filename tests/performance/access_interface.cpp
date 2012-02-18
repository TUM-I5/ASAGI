#include <asagi.h>

#include "perftests.h"

using namespace asagi;

int main()
{
	Grid* grid = Grid::create(); // FLOAT is default
	
	if (!grid->open(NC_FILENAME))
		return 1;
	
	float sum = 0;
	for (unsigned int i = 0; i < COUNT_ACCESS; i++) {
		sum += grid->getFloat(INDEX2D[i*2], INDEX2D[i*2+1]);
	}
	
	delete grid;
	
	// Force compiler to execute loop
	if (sum < 0)
		return 1;
	
	return 0;
}