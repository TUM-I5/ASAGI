#include <asagi.h>
#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
// #include <sys/unistd.h>

int main (int argc, char** argv)
{
	MPI_Init(&argc, &argv);
	
	asagi_grid* grid = asagi_grid_create(ASAGI_FLOAT);
	
	if (asagi_grid_open(grid, "tests/2dgrid.nc", 0) != ASAGI_SUCCESS) {
		printf("Could not load file\n");
		return 1;
	}
	
	printf("Range X: %f-%f\n", asagi_grid_min(grid, 0), asagi_grid_max(grid, 0));
	printf("Range Y: %f-%f\n", asagi_grid_min(grid, 1), asagi_grid_max(grid, 1));
	
	double pos[] = {5, 10};
	printf("Value at 5x10: %f\n", asagi_grid_get_float(grid, pos, 0));
	
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
	
	asagi_grid_close(grid);
	
	MPI_Finalize();
	
	return 0;
}
