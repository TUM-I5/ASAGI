#include <stdlib.h>
#include <stdio.h>
#include <sys/unistd.h>
#include <database.h>

int main (void)
{
	grid_handle grid;
	
	if (!(grid = grid_load("../data/tohoku_1850m_bath.nc"))) {
		printf("Could not load file\n");
		return 1;
	}
	
	printf("Dim (x*y): %u*%u\n", (unsigned int) grid_x(grid), (unsigned int) grid_y(grid));
	
	printf("Value at 5x10: %f\n", grid_get_value(grid, 5, 10));
	
	// Print memory usage
	char status_path[100];
	snprintf(status_path, 99, "/proc/%d/status", getpid());
	
	FILE* f = fopen(status_path, "r");
	
	char buf[256];
	while (fgets(buf, sizeof buf, f)) {
		printf("%s", buf);
	}
	fclose(f);
	
	return 0;
}