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
	virtual unsigned long getXDim() = 0;
	virtual unsigned long getYDim() = 0;
	virtual float get(unsigned long x, unsigned long y) = 0;
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

unsigned long grid_x(grid_handle* handle);
unsigned long grid_y(grid_handle* handle);

float grid_get_value(grid_handle* handle, unsigned long x, unsigned long y);

#ifdef __cplusplus
}	// end extern "C"
#endif

#endif // ifndef _DATABASE_H