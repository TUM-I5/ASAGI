#ifndef _DATABASE_H
#define _DATABASE_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
namespace grid {
	class Grid;
}

class grid::Grid
{
public:
	virtual bool open(const char* filename) = 0;
	virtual float getXMin() = 0;
	virtual float getYMin() = 0;
	virtual float getXMax() = 0;
	virtual float getYMax() = 0;
	virtual float get(float x, float y) = 0;
	
	virtual bool exportPng(const char* filename) = 0;
public:
	static grid::Grid* create();
};

typedef grid::Grid grid_handle;
#else
typedef struct grid_handle grid_handle;
#endif

/**
 * Load a grid form an nc file
 */
grid_handle* grid_load(const char* filename);

float grid_min_x(grid_handle* handle);
float grid_min_y(grid_handle* handle);
float grid_max_x(grid_handle* handle);
float grid_max_y(grid_handle* handle);

float grid_get_value(grid_handle* handle, float x, float y);

void grid_free(grid_handle* handle);

#ifdef __cplusplus
}	// end extern "C"
#endif

#endif // ifndef _DATABASE_H