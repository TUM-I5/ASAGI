/**
 * @file
 *  This file is part of ASAGI.
 * 
 *  ASAGI is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  ASAGI is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with ASAGI.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Diese Datei ist Teil von ASAGI.
 *
 *  ASAGI ist Freie Software: Sie koennen es unter den Bedingungen
 *  der GNU General Public License, wie von der Free Software Foundation,
 *  Version 3 der Lizenz oder (nach Ihrer Option) jeder spaeteren
 *  veroeffentlichten Version, weiterverbreiten und/oder modifizieren.
 *
 *  ASAGI wird in der Hoffnung, dass es nuetzlich sein wird, aber
 *  OHNE JEDE GEWAEHELEISTUNG, bereitgestellt; sogar ohne die implizite
 *  Gewaehrleistung der MARKTFAEHIGKEIT oder EIGNUNG FUER EINEN BESTIMMTEN
 *  ZWECK. Siehe die GNU General Public License fuer weitere Details.
 *
 *  Sie sollten eine Kopie der GNU General Public License zusammen mit diesem
 *  Programm erhalten haben. Wenn nicht, siehe <http://www.gnu.org/licenses/>.
 * 
 * @copyright 2012 Sebastian Rettenberger <rettenbs@in.tum.de>
 * @version \$Id$
 * 
 * @brief Include file for C and C++ API
 * 
 * @defgroup c_interface C Interface
 * @defgroup cxx_interface C++ Interface
 */

#ifndef _ASAGI_H
#define _ASAGI_H

#ifndef ASAGI_NOMPI
#include <mpi.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus

/**
 * @internal
 * 
 * Contains all the exported C++ functionality of the library
 */
namespace asagi {
	class Grid;
}

/**
 * @ingroup cxx_interface
 * 
 * @brief C++ Interface for ASAGI
 */
class asagi::Grid
{
public:
	/**
	 * @ingroup cxx_interface
	 * 
	 * The basic types supported by ASAGI
	 */
	enum Type {
		/** signed byte */
		BYTE,
		/** signed 4-byte integer */
		INT,
		/** signed 8-byte integer */
		LONG,
		/** 4-byte floating point value */
		FLOAT,
		/** 8-byte floating point value */
		DOUBLE
	};
	
	/**
	 * @ingroup cxx_interface
	 * 
	 * Possible errors that could occure
	 */
	enum Error {
		/** No error */
		SUCCESS = 0,
		/** An MPI function failed */
		MPI_ERROR,
		/** Unknown configuration parameter */
		UNKNOWN_PARAM,
		/** Invalid configuration value */
		INVALID_VALUE,
		/** Could not open input file */
		NOT_OPEN,
		/** netCDF variable not found */
		VAR_NOT_FOUND,
		/** Unsupported number of dimensions input file */
		UNSUPPORTED_DIMENSIONS,
		/** More than one topmost grid specified */
		MULTIPLE_TOPGRIDS,
		/** Variable size in the input file does not match the type */
		INVALID_VAR_SIZE };
	
	/** Does not provide any hint for ASAGI (default) */
	static const unsigned int NO_HINT = 0;
	/** One dimension in the grid is a time dimension */
	static const unsigned int HAS_TIME = 1;
	/** Use this, if you are going to load a large grid with ASAGI */
	static const unsigned int LARGE_GRID = 2;
	/**
	 * Use an adaptive container. Allows you to load multiple grids with
	 * the same level of detail. Not fully tested yet.
	 */
	static const unsigned int ADAPTIVE = 4;
public:
	/**
	 * @ingroup cxx_interface
	 * 
	 * @see close(asagi::Grid*)
	 */
	virtual ~Grid() {}	// This one does nothing, but destructors of
				// child classes are only called if we have it
				// here
	
#ifndef ASAGI_NOMPI
	/**
	 * @ingroup cxx_interface
	 * 
	 * Call this function before {@link open()} if another communicator
	 * than the default MPI_COMM_WORLD should be used.
	 */
	virtual Error setComm(MPI_Comm comm) = 0;
#endif
	/**
	 * @ingroup cxx_interface
	 * 
	 * @brief Changes a grid parameter
	 * 
	 * This function allows you to change ASAGI's configuration. It must
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
	 * @li @b block-cache-size Number of blocks cached on each node
	 * (Default: 80)
	 * @li @b cache-hand-spread The difference between the hands of the
	 * 2-handed clock algorithm (Default: block-cache-size/2)
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
	virtual double getXMin() const = 0;
	/**
	 * @ingroup cxx_interface
	 * 
	 * @return The minimum allowed coordinate in y dimension
	 */
	virtual double getYMin() const = 0;
	/**
	 * @ingroup cxx_interface
	 * 
	 * @return The minimum allowed coordinate in z dimension
	 */
	virtual double getZMin() const = 0;
	/**
	 * @ingroup cxx_interface
	 * 
	 * @return The maxmium allowed coordinate in x dimension
	 */
	virtual double getXMax() const = 0;
	/**
	 * @ingroup cxx_interface
	 * 
	 * @return The maximum allowed coordinate in y dimension
	 */
	virtual double getYMax() const = 0;
	/**
	 * @ingroup cxx_interface
	 * 
	 * @return The maximum allowed coordinate in z dimension
	 */
	virtual double getZMax() const = 0;
	
	/**
	 * @ingroup cxx_interface
	 * 
	 * @return The number of bytes that are stored in each grid cell
	 */
	virtual unsigned int getVarSize() const = 0;
	
	/********* 1D ********/
	
	/**
	 * @ingroup cxx_interface
	 * 
	 * @see getByte2D
	 */
	virtual char getByte1D(double x, unsigned int level = 0) = 0;
	/**
	 * @ingroup cxx_interface
	 * 
	 * @see getInt2D
	 */
	virtual int getInt1D(double x, unsigned int level = 0) = 0;
	/**
	 * @ingroup cxx_interface
	 * 
	 * @see getLong2D
	 */
	virtual long getLong1D(double x, unsigned int level = 0) = 0;
	/**
	 * @ingroup cxx_interface
	 * 
	 * @see getFloat2D
	 */
	virtual float getFloat1D(double x, unsigned int level = 0) = 0;
	/**
	 * @ingroup cxx_interface
	 * 
	 * @see getDouble2D
	 */
	virtual double getDouble1D(double x, unsigned int level = 0) = 0;
	/**
	 * @ingroup cxx_interface
	 * 
	 * @see getBuf2D
	 */
	virtual void getBuf1D(void* buf, double x, unsigned int level = 0) = 0;
	
	/******** 2D *********/
	
	/**
	 * @ingroup cxx_interface
	 * 
	 * If the grid contains array values, only the first element of the
	 * array is returned
	 * 
	 * @return The element at (x,y) as a char
	 */
	virtual char getByte2D(double x, double y, unsigned int level = 0) = 0;
	/**
	 * @ingroup cxx_interface
	 * 
	 * @return The element at (x,y) as an integer
	 * 
	 * @see getByte2D
	 */
	virtual int getInt2D(double x, double y, unsigned int level = 0) = 0;
	/**
	 * @ingroup cxx_interface
	 * 
	 * @return The element at (x,y) as a long
	 * 
	 * @see getByte2D
	 */
	virtual long getLong2D(double x, double y, unsigned int level = 0) = 0;
	/**
	 * @ingroup cxx_interface
	 * 
	 * @return The element at (x,y) as a float
	 * 
	 * @see getByte2D
	 */
	virtual float getFloat2D(double x, double y,
		unsigned int level = 0) = 0;
	/**
	 * @ingroup cxx_interface
	 * 
	 * @return The element at (x,y) as a double
	 * 
	 * @see getByte2D
	 */
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
	/**
	 * @ingroup cxx_interface
	 * 
	 * @see getByte2D
	 */
	
	/******** 3D ********/
	
	virtual char getByte3D(double x, double y, double z,
		unsigned int level = 0) = 0;
	/**
	 * @ingroup cxx_interface
	 * 
	 * @see getInt2D
	 */
	virtual int getInt3D(double x, double y, double z,
		unsigned int level = 0) = 0;
	/**
	 * @ingroup cxx_interface
	 * 
	 * @see getLong2D
	 */
	virtual long getLong3D(double x, double y, double z,
		unsigned int level = 0) = 0;
	/**
	 * @ingroup cxx_interface
	 * 
	 * @see getFloat2D
	 */
	virtual float getFloat3D(double x, double y, double z,
		unsigned int level = 0) = 0;
	/**
	 * @ingroup cxx_interface
	 * 
	 * @see getDouble2D
	 */
	virtual double getDouble3D(double x, double y, double z,
		unsigned int level = 0) = 0;
	/**
	 * @ingroup cxx_interface
	 * 
	 * @see getBuf2D
	 */
	virtual void getBuf3D(void* buf, double x, double y, double z,
		unsigned int level = 0) = 0;
	
	/**
	 * Exports the grid to png file. Should not be used for 3D grids
	 */
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

/// @cond deprecated_interface
namespace asagi {
	/**
	 * @deprecated Will be removed in further versions
	 *  use {@link asagi::Grid::NO_HINT}
	 */
	const unsigned int NO_HINT = Grid::NO_HINT;
	const unsigned int HAS_TIME = Grid::HAS_TIME;
	const unsigned int LARGE_GRID = Grid::LARGE_GRID;
	const unsigned int ADAPTIVE = Grid::ADAPTIVE;
}
/// @endcond

typedef asagi::Grid grid_handle;
typedef asagi::Grid::Type grid_type;
typedef asagi::Grid::Error grid_error;
#else
/**
 * @ingroup c_interface
 * 
 * @see asagi::Grid::NO_HINT
 */
const unsigned int GRID_NO_HINT = 0;
/**
 * @ingroup c_interface
 * 
 * @see asagi::Grid::HAS_TIME
 */
const unsigned int GRID_HAS_TIME = 1;
/**
 * @ingroup c_interface
 * 
 * @see asagi::Grid::LARGE_GRID
 */
const unsigned int GRID_LARGE_GRID = 2;
/**
 * @ingroup c_interface
 * 
 * @see asagi::Grid::ADAPTIVE
 */
const unsigned int GRID_ADAPTIVE = 4;

/**
 * @ingroup c_interface
 * 
 * A handle for a grid
 */
typedef struct grid_handle grid_handle;
/**
 * @ingroup c_interface
 * 
 * @see asagi::Grid::Type
 */
typedef enum { GRID_BYTE, GRID_INT, GRID_LONG, GRID_FLOAT, GRID_DOUBLE } grid_type;
/**
 * @ingroup c_interface
 * 
 * @see asagi::Grid::Error
 */
typedef enum { GRID_SUCCESS = 0, GRID_MPI_ERROR, GRID_UNKNOWN_PARAM,
	GRID_INVALID_VALUE, GRID_NOT_OPEN, GRID_VAR_NOT_FOUND,
	GRID_UNSUPPORTED_DIMENSIONS, GRID_MULTIPLE_TOPGRIDS,
	GRID_INVALID_VAR_SIZE } grid_error;
#endif


/**
 * @ingroup c_interface
 * 
 * @see asagi::Grid::create()
 */
grid_handle* grid_create(grid_type type, unsigned int hint,
	unsigned int levels);
/**
 * @ingroup c_interface
 * 
 * @see asagi::Grid::createArray()
 */
grid_handle* grid_create_array(grid_type basic_type, unsigned int hint,
	unsigned int levels);

#ifndef ASAGI_NOMPI
/**
 * @ingroup c_interface
 * 
 * @see asagi::Grid::setComm()
 */
grid_error grid_set_comm(grid_handle* handle, MPI_Comm comm);
#endif
/**
 * @ingroup c_interface
 * 
 * @see asagi::Grid::setParam()
 */
grid_error grid_set_param(grid_handle* handle, const char* name,
	const char* value, unsigned int level);
/**
 * @ingroup c_interface
 * 
 * @see asagi::Grid::open()
 */
grid_error grid_open(grid_handle* handle, const char* filename,
	unsigned int level);

/**
 * @ingroup c_interface
 * 
 * @see asagi::Grid::getXMin()
 */
double grid_min_x(grid_handle* handle);
/**
 * @ingroup c_interface
 *
 * @see asagi::Grid::getXMax()
 */
double grid_min_y(grid_handle* handle);
/**
 * @ingroup c_interface
 *
 * @see asagi::Grid::getYMin()
 */
double grid_min_z(grid_handle* handle);
/**
 * @ingroup c_interface
 *
 * @see asagi::Grid::getYMax()
 */
double grid_max_x(grid_handle* handle);
/**
 * @ingroup c_interface
 *
 * @see asagi::Grid::getZMin()
 */
double grid_max_y(grid_handle* handle);
/**
 * @ingroup c_interface
 *
 * @see asagi::Grid::getZMax()
 */
double grid_max_z(grid_handle* handle);

/**
 * @ingroup c_interface
 * 
 * @see asagi::Grid::getByte1D()
 */
char grid_get_byte_1d(grid_handle* handle, double x, unsigned int level);
/**
 * @ingroup c_interface
 * 
 * @see asagi::Grid::getInt1D()
 */
int grid_get_int_1d(grid_handle* handle, double x, unsigned int level);
/**
 * @ingroup c_interface
 * 
 * @see asagi::Grid::getLong1D()
 */
long grid_get_long_1d(grid_handle* handle, double x, unsigned int level);
/**
 * @ingroup c_interface
 * 
 * @see asagi::Grid::getFloat1D()
 */
float grid_get_float_1d(grid_handle* handle, double x, unsigned int level);
/**
 * @ingroup c_interface
 * 
 * @see asagi::Grid::getDouble1D()
 */
double grid_get_double_1d(grid_handle* handle, double x, unsigned int level);
/**
 * @ingroup c_interface
 * 
 * @see asagi::Grid::getBuf1D()
 */
void grid_get_buf_1d(grid_handle* handle,void* buf, double x,
	unsigned int level);

/**
 * @ingroup c_interface
 * 
 * @see asagi::Grid::getByte2D()
 */
char grid_get_byte_2d(grid_handle* handle, double x, double y,
	unsigned int level);
/**
 * @ingroup c_interface
 * 
 * @see asagi::Grid::getInt2D()
 */
int grid_get_int_2d(grid_handle* handle, double x, double y,
	unsigned int level);
/**
 * @ingroup c_interface
 * 
 * @see asagi::Grid::getLong2D()
 */
long grid_get_long_2d(grid_handle* handle, double x, double y,
	unsigned int level);
/**
 * @ingroup c_interface
 * 
 * @see asagi::Grid::getFloat2D()
 */
float grid_get_float_2d(grid_handle* handle, double x, double y,
	unsigned int level);
/**
 * @ingroup c_interface
 * 
 * @see asagi::Grid::getDouble2D()
 */
double grid_get_double_2d(grid_handle* handle, double x, double y,
	unsigned int level);
/**
 * @ingroup c_interface
 * 
 * @see asagi::Grid::getBuf2D()
 */
void grid_get_buf_2d(grid_handle* handle,void* buf, double x, double y,
	unsigned int level);

/**
 * @ingroup c_interface
 * 
 * @see asagi::Grid::getByte3D()
 */
char grid_get_byte_3d(grid_handle* handle, double x, double y, double z,
	unsigned int level);
/**
 * @ingroup c_interface
 * 
 * @see asagi::Grid::getInt3D()
 */
int grid_get_int_3d(grid_handle* handle, double x, double y, double z,
	unsigned int level);
/**
 * @ingroup c_interface
 * 
 * @see asagi::Grid::getLong3D()
 */
long grid_get_long_3d(grid_handle* handle, double x, double y, double z,
	unsigned int level);
/**
 * @ingroup c_interface
 * 
 * @see asagi::Grid::getFloat3D()
 */
float grid_get_float_3d(grid_handle* handle, double x, double y, double z,
	unsigned int level);
/**
 * @ingroup c_interface
 * 
 * @see asagi::Grid::getDouble3D()
 */
double grid_get_double_3d(grid_handle* handle, double x, double y, double z,
	unsigned int level);
/**
 * @ingroup c_interface
 * 
 * @see asagi::Grid::getBuf3D()
 */
void grid_get_buf_3d(grid_handle* handle,void* buf, double x, double y,
	double z, unsigned int level);

/**
 * @ingroup c_interface
 * 
 * @see asagi::Grid::close(asagi::Grid*)
 */
void grid_close(grid_handle* handle);

#ifdef __cplusplus
}
#endif

#endif