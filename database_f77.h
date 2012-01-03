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
GRID_GENERATE_F77_PROTOTYP(void, grid_x, GRID_X, (Fint* grid_id, Fint* x))
GRID_GENERATE_F77_PROTOTYP(void, grid_y, GRID_Y, (Fint* grid_id, Fint* y))
GRID_GENERATE_F77_PROTOTYP(void, grid_get_value, GRID_GET_VALUE, (Fint* grid_id, Fint* x, Fint* y, Freal* value))

#ifdef __cplusplus
}	// end extern "C"
#endif

#endif // ifndef _DATABASE_F77_H