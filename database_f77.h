#ifndef _DATABASE_F77_H
#define _DATABASE_F77_H

#define GRID_GENERATE_F77_PROTOTYP(ret, lower_name, upper_name, args) \
	ret f##lower_name##_f args;    \
	ret f##lower_name args;        \
	ret f##lower_name##_ args;     \
	ret f##lower_name##__ args;    \
	ret F##upper_name args;        \

#ifdef __cplusplus
extern "C" {
#endif

typedef int Fint;
typedef float Freal;

GRID_GENERATE_F77_PROTOTYP(void, grid_load, GRID_LOAD, (char* filename, Fint* grid_id, int filename_len))
GRID_GENERATE_F77_PROTOTYP(void, grid_min_x, GRID_MIN_X, (Fint* grid_id, Freal* x))
GRID_GENERATE_F77_PROTOTYP(void, grid_min_y, GRID_MIN_Y, (Fint* grid_id, Freal* y))
GRID_GENERATE_F77_PROTOTYP(void, grid_max_x, GRID_MAX_X, (Fint* grid_id, Freal* x))
GRID_GENERATE_F77_PROTOTYP(void, grid_max_y, GRID_MAX_Y, (Fint* grid_id, Freal* y))
GRID_GENERATE_F77_PROTOTYP(void, grid_get_value, GRID_GET_VALUE, (Fint* grid_id, Freal* x, Freal* y, Freal* value))

#ifdef __cplusplus
}	// end extern "C"
#endif

#endif // ifndef _DATABASE_F77_H