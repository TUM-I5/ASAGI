#include "basicgrid.h"
#include "perftests.h"

int main()
{
	BasicGrid<float> grid;
	
	if (!grid.open(NC_FILENAME))
		return 1;
	
	float sum = 0;
	for (unsigned int i = 0; i < COUNT_ACCESS; i++) {
		sum += grid.getNoDefault(
			static_cast<float>(INDEX2D[i*2]),
			static_cast<float>(INDEX2D[i*2+1]));
	}
	
	// Force compiler to execute loop
	if (sum < 0)
		return 1;
	
	return 0;
}