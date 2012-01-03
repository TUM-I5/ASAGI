#ifndef ASAGI_F95_H
#define ASAGI_F95_H

#include <asagi.h>

#ifdef __cplusplus
extern "C" {
#endif

int f95grid_create(grid_type type);

bool f95grid_open(int grid_id, const char *filename);

float f95grid_min_x(int grid_id);
float f95grid_min_y(int grid_id);
float f95grid_max_x(int grid_id);
float f95grid_max_y(int grid_id);

float f95grid_get_float(int grid_id, float x, float y);

void f95grid_free(int grid_id);

#ifdef __cplusplus
}	// end extern "C"
#endif

#endif // ifndef ASAGI_F95_H