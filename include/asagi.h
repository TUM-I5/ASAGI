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
	
	const unsigned int NO_HINT = 0;
	const unsigned int HAS_TIME = 1;
}

class asagi::Grid
{
public:
	enum Type { BYTE, INT, LONG, FLOAT, DOUBLE };
	enum Error { SUCCESS = 0, MPI_ERROR, UNKNOWN_PARAM, INVALID_VALUE,
		NOT_OPEN, VAR_NOT_FOUND, UNSUPPORTED_DIMENSIONS,
		INVALID_VAR_SIZE };
public:
	virtual ~Grid();	// This one does nothing, but destructors of
				// child classes are only called if we have it
				// here
	
	virtual Error init(MPI_Comm comm = MPI_COMM_WORLD) = 0;
	virtual Error setParam(const char* name, const char* value,
		unsigned int level = 0) = 0;
	virtual Error open(const char* filename,
		unsigned int level = 0) = 0;

	virtual double getXMin() = 0;
	virtual double getYMin() = 0;
	virtual double getZMin() = 0;
	virtual double getXMax() = 0;
	virtual double getYMax() = 0;
	virtual double getZMax() = 0;
	
	/**
	 * @return The number of bytes that are stored in each grid cell
	 */
	virtual unsigned int getVarSize() = 0;
	
	virtual char getByte1D(double x, unsigned int level = 0) = 0;
	virtual int getInt1D(double x, unsigned int level = 0) = 0;
	virtual long getLong1D(double x, unsigned int level = 0) = 0;
	virtual float getFloat1D(double x, unsigned int level = 0) = 0;
	virtual double getDouble1D(double x, unsigned int level = 0) = 0;
	/**
	 * @see #getBuf2D
	 */
	virtual void getBuf1D(void* buf, double x, unsigned int level = 0) = 0;
	
	virtual char getByte2D(double x, double y, unsigned int level = 0) = 0;
	virtual int getInt2D(double x, double y, unsigned int level = 0) = 0;
	virtual long getLong2D(double x, double y, unsigned int level = 0) = 0;
	virtual float getFloat2D(double x, double y,
		unsigned int level = 0) = 0;
	virtual double getDouble2D(double x, double y,
		unsigned int level = 0) = 0;
	/**
	 * Copys the element at (x,y) into buf. The buffer size has to be
	 * (at least) {@link getVarSize()} bytes. 
	 */
	virtual void getBuf2D(void* buf, double x, double y,
		unsigned int level = 0) = 0;
	
	virtual char getByte3D(double x, double y, double z,
		unsigned int level = 0) = 0;
	virtual int getInt3D(double x, double y, double z,
		unsigned int level = 0) = 0;
	virtual long getLong3D(double x, double y, double z,
		unsigned int level = 0) = 0;
	virtual float getFloat3D(double x, double y, double z,
		unsigned int level = 0) = 0;
	virtual double getDouble3D(double x, double y, double z,
		unsigned int level = 0) = 0;
	/**
	 * @see #getBuf2D
	 */
	virtual void getBuf3D(void* buf, double x, double y, double z,
		unsigned int level = 0) = 0;
	
	virtual bool exportPng(const char* filename,
		unsigned int level = 0) = 0;
public:
	/**
	 * Creates a new grid
	 * 
	 * @param type The type of the grid
	 * @param hint A combination of hints
	 * @param level The number of level this grid should have
	 */
	static asagi::Grid* create(Type type = FLOAT,
		unsigned int hint = NO_HINT, unsigned int levels = 1);
};

typedef asagi::Grid grid_handle;
typedef asagi::Grid::Type grid_type;
typedef asagi::Grid::Error grid_error;
#else
const unsigned int GRID_NO_HINT = 0;
const unsigned int GRID_HAS_TIME = 1;

typedef struct grid_handle grid_handle;
typedef enum { GRID_BYTE, GRID_INT, GRID_LONG, GRID_FLOAT, GRID_DOUBLE } grid_type;
typedef enum { GRID_SUCCESS = 0, GRID_MPI_ERROR, GRID_UNKNOWN_PARAM,
	GRID_INVALID_VALUE, GRID_NOT_OPEN, GRID_VAR_NOT_FOUND,
	GRID_UNSUPPORTED_DIMENSIONS, GRID_INVALID_VAR_SIZE } grid_error;
#endif

/**
 * Load a grid form an nc file
 */
grid_handle* grid_create(grid_type type, unsigned int hint,
	unsigned int levels);

grid_error grid_init(grid_handle* handle, MPI_Comm comm);
grid_error grid_set_param(grid_handle* handle, const char* name,
	const char* value, unsigned int level);
grid_error grid_open(grid_handle* handle, const char* filename,
	unsigned int level);

double grid_min_x(grid_handle* handle);
double grid_min_y(grid_handle* handle);
double grid_min_z(grid_handle* handle);
double grid_max_x(grid_handle* handle);
double grid_max_y(grid_handle* handle);
double grid_max_z(grid_handle* handle);

char grid_get_byte_1d(grid_handle* handle, double x, unsigned int level);
int grid_get_int_1d(grid_handle* handle, double x, unsigned int level);
long grid_get_long_1d(grid_handle* handle, double x, unsigned int level);
float grid_get_float_1d(grid_handle* handle, double x, unsigned int level);
double grid_get_double_1d(grid_handle* handle, double x, unsigned int level);
void grid_get_buf_1d(grid_handle* handle,void* buf, double x,
	unsigned int level);

char grid_get_byte_2d(grid_handle* handle, double x, double y,
	unsigned int level);
int grid_get_int_2d(grid_handle* handle, double x, double y,
	unsigned int level);
long grid_get_long_2d(grid_handle* handle, double x, double y,
	unsigned int level);
float grid_get_float_2d(grid_handle* handle, double x, double y,
	unsigned int level);
double grid_get_double_2d(grid_handle* handle, double x, double y,
	unsigned int level);
void grid_get_buf_2d(grid_handle* handle,void* buf, double x, double y,
	unsigned int level);

char grid_get_byte_3d(grid_handle* handle, double x, double y, double z,
	unsigned int level);
int grid_get_int_3d(grid_handle* handle, double x, double y, double z,
	unsigned int level);
long grid_get_long_3d(grid_handle* handle, double x, double y, double z,
	unsigned int level);
float grid_get_float_3d(grid_handle* handle, double x, double y, double z,
	unsigned int level);
double grid_get_double_3d(grid_handle* handle, double x, double y, double z,
	unsigned int level);
void grid_get_buf_3d(grid_handle* handle,void* buf, double x, double y, double z,
	unsigned int level);

void grid_free(grid_handle* handle);

#ifdef __cplusplus
}	// end extern "C"
#endif

#endif // ifndef _ASAGI_H