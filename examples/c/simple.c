#include <asagi.h>
#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
// #include <sys/unistd.h>

int main (int argc, char** argv)
{
	MPI_Init(&argc, &argv);
	
	grid_handle* grid = grid_create(GRID_FLOAT, GRID_NO_HINT, 1);
	
	if (grid_open(grid, "../data/tohoku_1850m_bath.nc", 0) != GRID_SUCCESS) {
		printf("Could not load file\n");
		return 1;
	}
	
	printf("Range X: %f-%f\n", grid_min_x(grid), grid_max_x(grid));
	printf("Range Y: %f-%f\n", grid_min_y(grid), grid_max_y(grid));
	
	printf("Value at 5x10: %f\n", grid_get_float_2d(grid, 5, 10, 0));
	
	// Print memory usage
// 	char status_path[100];
// 	snprintf(status_path, 99, "/proc/%d/status", getpid());
// 	
// 	FILE* f = fopen(status_path, "r");
// 	
// 	char buf[256];
// 	while (fgets(buf, sizeof buf, f)) {
// 		printf("%s", buf);
// 	}
// 	fclose(f);
	
	grid_close(grid);
	
	MPI_Finalize();
	
	return 0;
}