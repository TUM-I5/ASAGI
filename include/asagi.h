#ifndef _ASAGI_H
#define _ASAGI_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
/**
 * Contains all the exported C++ functionality of the library
 */
namespace asagi {
	class Grid;
	class VarType;
}

class asagi::Grid
{
public:
	enum Type { BYTE, INT, LONG, FLOAT, DOUBLE, BYTEARRAY };
public:
	virtual bool open(const char* filename) = 0;
	virtual float getXMin() = 0;
	virtual float getYMin() = 0;
	virtual float getXMax() = 0;
	virtual float getYMax() = 0;
	virtual float getFloat(float x, float y) = 0;
	virtual double getDouble(float x, float y) = 0;
	
	virtual bool exportPng(const char* filename) = 0;
public:
	static asagi::Grid* create(Type type = FLOAT);
};

typedef asagi::Grid grid_handle;
typedef asagi::Grid::Type grid_type;
#else
typedef struct grid_handle grid_handle;
typedef enum { GRID_BYTE, GRID_INT, GRID_LONG, GRID_FLOAT, GRID_DOUBLE, GRID_VOID } grid_type;
#endif

/**
 * Load a grid form an nc file
 */
grid_handle* grid_create(grid_type type);

int grid_open(grid_handle* handle, const char* filename);

float grid_min_x(grid_handle* handle);
float grid_min_y(grid_handle* handle);
float grid_max_x(grid_handle* handle);
float grid_max_y(grid_handle* handle);

float grid_get_float(grid_handle* handle, float x, float y);

void grid_free(grid_handle* handle);

#ifdef __cplusplus
}	// end extern "C"
#endif

#endif // ifndef _ASAGI_H