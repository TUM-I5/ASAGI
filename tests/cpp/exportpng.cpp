#include <asagi.h>
#include <stdio.h>

using namespace asagi;

int main (void)
{
#ifdef WITH_PNG
	Grid* grid = Grid::create();
	
	if (!grid->open("../data/noise.nc")) {
		printf("Could not load file\n");
		return 1;
	}
	
	if (!grid->exportPng("../data/export.png")) {
		printf("Could not write png file\n");
		return 1;
	}
	
	delete grid;
	
	grid = Grid::create();
	
	if (!grid->open("../data/tohoku_1850m_bath.nc")) {
	//if (!grid->open("../data/noise.nc")) {
		printf("Could not load file\n");
		return 1;
	}
	
	if (!grid->exportPng("../data/export2.png")) {
		printf("Could not write png file\n");
		return 1;
	}
	
	printf("Png file written\n");
#else // WITH_PNG
	printf("Png export not supported\n");
#endif // WITH_PNG
	
	return 0;
}