#ifndef ASAGI_F95_H
#define ASAGI_F95_H

#include <asagi.h>

#ifdef __cplusplus
extern "C" {
#endif

int f90grid_create(grid_type type, int hint,
	int levels);

grid_error f90grid_set_comm(int grid_id, int comm);
grid_error f90grid_set_param(int grid_id, const char* name,
	const char* value, int level);
grid_error f90grid_open(int grid_id, const char* filename,
	int level);

double f90grid_min_x(int grid_id);
double f90grid_min_y(int grid_id);
double f90grid_min_z(int grid_id);
double f90grid_max_x(int grid_id);
double f90grid_max_y(int grid_id);
double f90grid_max_z(int grid_id);

char f90grid_get_byte_1d(int grid_id, double x, int level);
int f90grid_get_int_1d(int grid_id, double x, int level);
long f90grid_get_long_1d(int grid_id, double x, int level);
float f90grid_get_float_1d(int grid_id, double x, int level);
double f90grid_get_double_1d(int grid_id, double x, int level);
void f90grid_get_buf_1d(int grid_id,void* buf, double x,
	int level);

char f90grid_get_byte_2d(int grid_id, double x, double y,
	int level);
int f90grid_get_int_2d(int grid_id, double x, double y,
	int level);
long f90grid_get_long_2d(int grid_id, double x, double y,
	int level);
float f90grid_get_float_2d(int grid_id, double x, double y,
	int level);
double f90grid_get_double_2d(int grid_id, double x, double y,
	int level);
void f90grid_get_buf_2d(int grid_id,void* buf, double x, double y,
	int level);

char f90grid_get_byte_3d(int grid_id, double x, double y, double z,
	int level);
int f90grid_get_int_3d(int grid_id, double x, double y, double z,
	int level);
long f90grid_get_long_3d(int grid_id, double x, double y, double z,
	int level);
float f90grid_get_float_3d(int grid_id, double x, double y, double z,
	int level);
double f90grid_get_double_3d(int grid_id, double x, double y, double z,
	int level);
void f90grid_get_buf_3d(int grid_id,void* buf, double x, double y, double z,
	int level);

void f90grid_close(int grid_id);

#ifdef __cplusplus
}	// end extern "C"
#endif

#endif // ifndef ASAGI_F95_H
