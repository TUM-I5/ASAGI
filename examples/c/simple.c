#include <asagi.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/unistd.h>

int main (void)
{
	grid_handle* grid = grid_create(GRID_FLOAT);
	
	if (!grid_open(grid, "../data/tohoku_1850m_bath.nc")) {
		printf("Could not load file\n");
		return 1;
	}
	
	printf("Range X: %f-%f\n", grid_min_x(grid), grid_max_x(grid));
	printf("Range Y: %f-%f\n", grid_min_y(grid), grid_max_y(grid));
	
	printf("Value at 5x10: %f\n", grid_get_float(grid, 5, 10));
	
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