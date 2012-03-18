/**
 * @file
 * @author Sebastian Rettenberger <rettenbs@in.tum.de>
 * 
 * Include file for C and C++ API
 * 
 * @defgroup c_interface C Interface
 * @defgroup cxx_interface C++ Interface
 */

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

/**
 * @ingroup cxx_interface
 */
class asagi::Grid
{
public:
	enum Type { BYTE, INT, LONG, FLOAT, DOUBLE };
	enum Error { SUCCESS = 0, MPI_ERROR, UNKNOWN_PARAM, INVALID_VALUE,
		NOT_OPEN, VAR_NOT_FOUND, UNSUPPORTED_DIMENSIONS,
		INVALID_VAR_SIZE };
public:
	/**
	 * @ingroup cxx_interface
	 * 
	 * @see #close(asagi::Grid*)
	 */
	virtual ~Grid();	// This one does nothing, but destructors of
				// child classes are only called if we have it
				// here
	
	virtual Error setComm(MPI_Comm comm) = 0;
	/**
	 * @ingroup cxx_interface
	 * 
	 * @brief Changes a grid parameter
	 * 
	 * This function allows you to change %ASAGI's configuration. It must
	 * be called before calling {@link open(const char*, unsigned int)}.
	 * 
	 * The following parameters are supported:
	 * @li @b value-position The value should be either @c cell-centered
	 * (default) or @c vertex-centered. <br> Note: This parameter does not
	 * depend on the level.
	 * @li @b variable-name The name of the variable in the NetCDF file
	 * (default: "z")
	 * @li @b time-dimension The dimension that holds the time. Only useful
	 * with the hint HAS_TIME. Should be either "x", "y" or "z". (Default:
	 * the last dimension of the grid)
	 * @li @b x-block-size The block size in x dimension (Default: 50)
	 * @li @b y-block-size The block size in y dimension (Default: 50 or
	 * 1 if it is an 1-dimensional grid)
	 * @li @b z-block-size The block size in z dimension (Default: 50 or
	 * 1 if it is an 1- or 2-dimensional grid)
	 * 
	 * @param name The name of the parameter
	 * @param value The new value for the parameter
	 * @param level Change the parameter for the specified level of detail.
	 * <br> Should be 0 when setting @b value-position
	 * @return @c SUCCESS if the parameter was successfully changed <br>
	 * @c UNKNOWN_PARAM if the parameter is not supported <br>
	 * @c INVALID_VALUE if the parameter does not accept this value
	 */
	virtual Error setParam(const char* name, const char* value,
		unsigned int level = 0) = 0;
	/**
	 * @ingroup cxx_interface
	 * 
	 * @brief Loads values from a NetCDF file
	 * 
	 * This function must be called for each level of detail
	 */
	virtual Error open(const char* filename,
		unsigned int level = 0) = 0;

	/**
	 * @ingroup cxx_interface
	 * 
	 * @return The minimum allowed coordinate in x dimension
	 */
	virtual double getXMin() = 0;
	/**
	 * @ingroup cxx_interface
	 * 
	 * @return The minimum allowed coordinate in y dimension
	 */
	virtual double getYMin() = 0;
	/**
	 * @ingroup cxx_interface
	 */
	virtual double getZMin() = 0;
	virtual double getXMax() = 0;
	virtual double getYMax() = 0;
	virtual double getZMax() = 0;
	
	/**
	 * @ingroup cxx_interface
	 * 
	 * @return The number of bytes that are stored in each grid cell
	 */
	virtual unsigned int getVarSize() = 0;
	
	virtual char getByte1D(double x, unsigned int level = 0) = 0;
	virtual int getInt1D(double x, unsigned int level = 0) = 0;
	virtual long getLong1D(double x, unsigned int level = 0) = 0;
	virtual float getFloat1D(double x, unsigned int level = 0) = 0;
	virtual double getDouble1D(double x, unsigned int level = 0) = 0;
	/**
	 * @ingroup cxx_interface
	 * 
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
	 * @ingroup cxx_interface
	 * 
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
	 * @ingroup cxx_interface
	 * 
	 * @see #getBuf2D
	 */
	virtual void getBuf3D(void* buf, double x, double y, double z,
		unsigned int level = 0) = 0;
	
	virtual bool exportPng(const char* filename,
		unsigned int level = 0) = 0;
public:
	/**
	 * @ingroup cxx_interface
	 * 
	 * Creates a new grid with basic values
	 * 
	 * @param type The type of the grid
	 * @param hint A combination of hints
	 * @param levels The number of level this grid should have
	 */
	static asagi::Grid* create(Type type = FLOAT,
		unsigned int hint = NO_HINT, unsigned int levels = 1);
	
	/**
	 * @ingroup cxx_interface
	 * 
	 * Creates a new grid with array values
	 * 
	 * @param basicType The type of the array values in the grid
	 * @param hint A combination of hints
	 * @param levels The number of levels this grid should have
	 */
	static asagi::Grid* createArray(Type basicType = FLOAT,
		unsigned int hint = NO_HINT, unsigned int levels = 1);
	
	/**
	 * @ingroup cxx_interface
	 * 
	 * Frees all memory resources assciated with @c grid. After a grid is
	 * closed you cannot access any values and you can not reopen another
	 * NetCDF file.
	 * <br>
	 * This function does the same as calling <code>delete grid;</code> and
	 * it is the C++ equivalent to {@link grid_close(grid_handle*)} and
	 * {@link ASAGI::grid_close}
	 * 
	 * @param grid The grid that should be closed.
	 */
	static void close(asagi::Grid* grid)
	{
		delete grid;
	}
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


grid_handle* grid_create(grid_type type, unsigned int hint,
	unsigned int levels);
grid_handle* grid_create_array(grid_type basic_type, unsigned int hint,
	unsigned int levels);

grid_error grid_set_comm(grid_handle* handle, MPI_Comm comm);
grid_error grid_set_param(grid_handle* handle, const char* name,
	const char* value, unsigned int level);
grid_error grid_open(grid_handle* handle, const char* filename,
	unsigned int level);

/**
 * @ingroup c_interface
 * 
 * @see asagi::Grid#getXMin()
 */
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

/**
 * @ingroup c_interface
 * 
 * @see asagi::Grid#close(asagi::Grid*)
 */
void grid_close(grid_handle* handle);

#ifdef __cplusplus
}	// end extern "C"
#endif

#endif // ifndef _ASAGI_H