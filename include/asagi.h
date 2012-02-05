#ifndef _ASAGI_H
#define _ASAGI_H

#include <mpi.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
/**
 * Contains all the exported C++ functionality of the library
 */
namespace asagi {
	class Grid;
}

class asagi::Grid
{
public:
	enum Type { BYTE, INT, LONG, FLOAT, DOUBLE,
		BYTEARRAY, INTARRAY, LONGARRY, FLOATARRAY, DOUBLEARRAY };
public:
	virtual ~Grid();	// This one does nothing, but destructors of
				// child classes are only called if we have it
				// here
	
	virtual bool open(const char* filename) = 0;
	virtual float getXMin() = 0;
	virtual float getYMin() = 0;
	virtual float getXMax() = 0;
	virtual float getYMax() = 0;
	
	/**
	 * @return The number of bytes that are stored at each grid node
	 */
	virtual unsigned int getVarSize() = 0;
	virtual char getByte(float x, float y) = 0;
	virtual int getInt(float x, float y) = 0;
	virtual long getLong(float x, float y) = 0;
	virtual float getFloat(float x, float y) = 0;
	virtual double getDouble(float x, float y) = 0;
	/**
	 * Copys the element at (x,y) into buf. The buffer size has to be
	 * (at least) {@link getVarSize()} bytes. 
	 */
	virtual void getBuf(float x, float y, void* buf) = 0;
	
	virtual bool exportPng(const char* filename) = 0;
public:
	/**
	 * This function should be called before calling any other function
	 * from this library.
	 */
	static bool init(MPI_Comm comm);
	static asagi::Grid* create(Type type = FLOAT);
	/**
	 * This function cleans up Asagi. After calling this function, no other
	 * function than {@link init(MPI_Comm)} should be called.
	 * <br>
	 * Call this <b>before</b> calling <code>MPI_Finalize()</code>
	 */
	static bool finalize();
};

typedef asagi::Grid grid_handle;
typedef asagi::Grid::Type grid_type;
#else
typedef struct grid_handle grid_handle;
typedef enum { GRID_BYTE, GRID_INT, GRID_LONG, GRID_FLOAT, GRID_DOUBLE,
	GRID_BYTEARRAY, GRID_INTARRAY, GRID_LONGARRAY,
	GRID_FLOATARRY, GRID_DOUBLEARRAY } grid_type;
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

char grid_get_char(grid_handle* handle, float x, float y);
int grid_get_int(grid_handle* handle, float x, float y);
long grid_get_long(grid_handle* handle, float x, float y);
float grid_get_float(grid_handle* handle, float x, float y);
double grid_get_double(grid_handle* handle, float x, float y);
void grid_get_buf(grid_handle* handle, float x, float y, void* buf);

void grid_free(grid_handle* handle);

#ifdef __cplusplus
}	// end extern "C"
#endif

#endif // ifndef _ASAGI_H