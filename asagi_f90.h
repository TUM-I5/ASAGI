#ifndef ASAGI_F95_H
#define ASAGI_F95_H

#include <asagi.h>

#ifdef __cplusplus
extern "C" {
#endif

int f90grid_create(grid_type type);

bool f90grid_open(int grid_id, const char *filename, int comm);

double f90grid_min_x(int grid_id);
double f90grid_min_y(int grid_id);
double f90grid_max_x(int grid_id);
double f90grid_max_y(int grid_id);

char f90grid_get_byte(int grid_id, double x, double y);
int f90grid_get_int(int grid_id, double x, double y);
long f90grid_get_long(int grid_id, double x, double y);
float f90grid_get_float(int grid_id, double x, double y);
double f90grid_get_double(int grid_id, double x, double y);
void f90grid_get_buf(int grid_id, double x, double y, void* buf);

void f90grid_free(int grid_id);

#ifdef __cplusplus
}	// end extern "C"
#endif

#endif // ifndef ASAGI_F95_H