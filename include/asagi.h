/**
 * @file
 *  This file is part of ASAGI.
 * 
 *  ASAGI is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as
 *  published by the Free Software Foundation, either version 3 of
 *  the License, or  (at your option) any later version.
 *
 *  ASAGI is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with ASAGI.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Diese Datei ist Teil von ASAGI.
 *
 *  ASAGI ist Freie Software: Sie koennen es unter den Bedingungen
 *  der GNU Lesser General Public License, wie von der Free Software
 *  Foundation, Version 3 der Lizenz oder (nach Ihrer Option) jeder
 *  spaeteren veroeffentlichten Version, weiterverbreiten und/oder
 *  modifizieren.
 *
 *  ASAGI wird in der Hoffnung, dass es nuetzlich sein wird, aber
 *  OHNE JEDE GEWAEHELEISTUNG, bereitgestellt; sogar ohne die implizite
 *  Gewaehrleistung der MARKTFAEHIGKEIT oder EIGNUNG FUER EINEN BESTIMMTEN
 *  ZWECK. Siehe die GNU Lesser General Public License fuer weitere Details.
 *
 *  Sie sollten eine Kopie der GNU Lesser General Public License zusammen
 *  mit diesem Programm erhalten haben. Wenn nicht, siehe
 *  <http://www.gnu.org/licenses/>.
 * 
 * @copyright 2012-2015 Sebastian Rettenberger <rettenbs@in.tum.de>
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
 * @brief C++ Interface for ASAGI grids
 */
class asagi::Grid
{
public:
	/**
	 * @ingroup cxx_interface
	 * 
	 * The primitive data types supported by ASAGI
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
	 * Possible errors that could occur
	 */
	enum Error {
		/** No error */
		SUCCESS = 0,
		/** An MPI function failed */
		MPI_ERROR,
		/** A pthread function failed */
		THREAD_ERROR,
		/** An error in the NUMA detection code */
		NUMA_ERROR,
		/** Unknown configuration parameter */
		UNKNOWN_PARAM,
		/** Invalid configuration value */
		INVALID_VALUE,
		/** Could not open input file */
		NOT_OPEN,
		/** netCDF variable not found */
		VAR_NOT_FOUND,
		/** Wrong variable size in the file */
		WRONG_SIZE,
		/** Unsupported number of dimensions input file */
		UNSUPPORTED_DIMENSIONS,
		/** More than one topmost grid specified */
		MULTIPLE_TOPGRIDS,
		/** Variable size in the input file does not match the type */
		INVALID_VAR_SIZE
	};
	
	/** Does not provide any hint for ASAGI (default) */
	static const unsigned int NO_HINT = 0x0;
	/** One dimension in the grid is a time dimension */
	static const unsigned int HAS_TIME = 0x1;
	/**
	 * Don't use any MPI, even when compiled with MPI support
	 * (MPI_Init may not be called before creating the grid)
	 */
	static const unsigned int NOMPI = 0x2;
	/**
	 * ASAGI should use a small cache. Less memory is used, but
	 * more cache misses occur.
	 */
	static const unsigned int SMALL_CACHE = 0x4;
	/** Use this, if you are going to load a large grid with ASAGI */
	static const unsigned int LARGE_GRID = 0x8;
	/**
	 * Use an adaptive container. Allows you to load multiple grids with
	 * the same level of detail. Not fully tested yet.
	 */
	static const unsigned int ADAPTIVE = 0x10;
	/**
	 * Use ASAGI only as a wrapper for the underlying I/O library.
	 * ASAGI does not cache any values. Works also without MPI.
	 */
	static const unsigned int PASS_THROUGH = 0x20;
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
	 * Call this function before {@link open()} if the grids should
	 * exchange chunks via MPI.
	 */
	virtual Error setComm(MPI_Comm comm = MPI_COMM_WORLD) = 0;
#endif

	/**
	 * @ingroup cxx_interface
	 *
	 * @brief Sets the number of threads in the application
	 *
	 * This function must be called before {@link open()}.
	 * If it is not called, one thread is assumed.
	 */
	virtual Error setThreads(unsigned int threads) = 0;

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
	 *  (default) or @c vertex-centered. <br> Note: This parameter does not
	 *  depend on the level.
	 * @li @b variable-name The name of the variable in the NetCDF file
	 *  (default: "z")
	 * @li @b time-dimension The dimension that holds the time. Only useful
	 *  with the hint HAS_TIME. Should be either "x", "y" or "z". (Default:
	 *  the last dimension of the grid)
	 * @li @b x-block-size The block size in x dimension (Default: 50)
	 * @li @b y-block-size The block size in y dimension (Default: 50 or
	 *  1 if it is an 1-dimensional grid)
	 * @li @b z-block-size The block size in z dimension (Default: 50 or
	 *  1 if it is an 1- or 2-dimensional grid)
	 * @li @b block-cache-size Number of blocks cached on each node
	 *  (Default: 80)
	 * @li @b cache-hand-spread The difference between the hands of the
	 *  2-handed clock algorithm (Default: block-cache-size/2)
	 * @li @b multigrid-size Sets the number of grids for the level.
	 *  Call this before setting any other parameter. (Default 1)
	 * 
	 * @param name The name of the parameter
	 * @param value The new value for the parameter
	 * @param level Change the parameter for the specified level of detail.
	 * <br> Should be 0 when setting @b value-position
	 */
	virtual void setParam(const char* name, const char* value,
		unsigned int level = 0) = 0;

	/**
	 * @ingroup cxx_interface
	 * 
	 * @brief Loads values from a NetCDF file
	 * 
	 * This function must be called for each level of detail.
	 * If more than one thread is used, this is a collective function
	 * for all threads.
	 */
	virtual Error open(const char* filename,
		unsigned int level = 0) = 0;

	/**
	 * @ingroup cxx_interface
	 * 
	 * @return The minimum allowed coordinate in dimension <code>n</code>
	 */
	virtual double getMin(unsigned int n) const = 0;

	/**
	 * @ingroup cxx_interface
	 * 
	 * @return The maximum allowed coordinate in dimension <code>n</code>
	 */
	virtual double getMax(unsigned int n) const = 0;
	
	/**
	 * @ingroup cxx_interface
	 * 
	 * @return The difference of two coordinates in dimension <code>n</code>
	 */
	virtual double getDelta(unsigned int n, unsigned int level = 0) const = 0;

	/**
	 * @ingroup cxx_interface
	 *
	 * @return The number of bytes that are stored in each grid cell
	 */
	virtual unsigned int getVarSize() const = 0;
	
	/**
	 * @ingroup cxx_interface
	 * 
	 * If the grid contains array values, only the first element of the
	 * array is returned
	 * 
	 * @param pos The coordinates of the value, the array must have at least
	 *  the size of the dimension of the grid
	 * @return The element at <code>pos</code> as a char
	 */
	virtual unsigned char getByte(const double* pos, unsigned int level = 0) = 0;
	/**
	 * @ingroup cxx_interface
	 * 
	 * @param pos The coordinates of the value, the array must have at least
	 *  the size of the dimension of the grid
	 * @return The element at <code>pos</code> as an integer
	 * 
	 * @see getByte
	 */
	virtual int getInt(const double* pos, unsigned int level = 0) = 0;
	/**
	 * @ingroup cxx_interface
	 * 
	 * @param pos The coordinates of the value, the array must have at least
	 *  the size of the dimension of the grid
	 * @return The element at <code>pos</code> as a long
	 * 
	 * @see getByte
	 */
	virtual long getLong(const double* pos, unsigned int level = 0) = 0;
	/**
	 * @ingroup cxx_interface
	 * 
	 * @param pos The coordinates of the value, the array must have at least
	 *  the size of the dimension of the grid
	 * @return The element at <code>pos</code> as a float
	 * 
	 * @see getByte
	 */
	virtual float getFloat(const double* pos, unsigned int level = 0) = 0;
	/**
	 * @ingroup cxx_interface
	 * 
	 * @param pos The coordinates of the value, the array must have at least
	 *  the size of the dimension of the grid
	 * @return The element at <code>pos</code> as a double
	 * 
	 * @see getByte
	 */
	virtual double getDouble(const double* pos, unsigned int level = 0) = 0;
	/**
	 * @ingroup cxx_interface
	 * 
	 * Copys the element at <code>pos</code> into buf. The buffer size has to be
	 * (at least) {@link getVarSize()} bytes.
	 * 
	 * @param pos The coordinates of the value, the array must have at least
	 *  the size of the dimension of the grid
	 */
	virtual void getBuf(void* buf, const double* pos, unsigned int level = 0) = 0;

	/**
	 * @ingroup cxx_interface
	 *
	 * Gets the current value of a counter for a grid level.
	 *
	 * Possible counter names:
	 * @li @b accesses Total number of data accesses
	 * @li @b mpi_transfers Number of blocks transfered between processes
	 * @li @b file_load Number of blocks loaded from file
	 *  (after initialization)
	 * @li @b local_hits Number values that where already in local memory
	 * @li @b local_misses Number of values that where not already in
	 *  local memory
	 *
	 *  @return The current counter value or 0 if the name is not defined
	 *
	 *  @warning The performance counters are not threadsafe for performance reason.
	 *   You may get wrong result when using more than one thread.
	 */
	virtual unsigned long getCounter(const char *name,
		unsigned int level = 0) = 0;

public:
	/**
	 * @ingroup cxx_interface
	 * 
	 * Creates a new grid containing values with a primitive
	 * data type
	 * 
	 * @param type The type of the values in the grid
	 */

	static asagi::Grid* create(Type type = FLOAT);
	/**
	 * @ingroup cxx_interface
	 *
	 * Creates a new grid containing arrays
	 *
	 * The length of the arrays is determined by the input file
	 *
	 * @param type The type of the values in the arrays
	 */
	static asagi::Grid* createArray(Type type = FLOAT);

	/**
	 * @ingroup cxx_interface
	 *
	 * Creates a new grid containing structured values
	 *
	 * @param count Number of blocks in the structure
	 * @param blockLength Number of elements in each block
	 * @param displacements Displacement of each block
	 * @param types Primitive types of the blocks
	 */
	static asagi::Grid* createStruct(unsigned int count,
		unsigned int blockLength[], unsigned long displacements[], Type types[]);
	
	/**
	 * @ingroup cxx_interface
	 * 
	 * Frees all memory resources assciated with @c grid. After a grid is
	 * closed you cannot access any values and you can not reopen another
	 * NetCDF file.
	 * <br>
	 * This function does the same as calling <code>delete grid;</code> and
	 * it is the C++ equivalent to {@link grid_close(asagi_grid*)} and
	 * {@link ASAGI::grid_close}
	 * 
	 * @param grid The grid that should be closed.
	 */
	static void close(asagi::Grid* grid)
	{
		delete grid;
	}
};

typedef asagi::Grid asagi_grid;
typedef asagi::Grid::Type asagi_type;
typedef asagi::Grid::Error asagi_error;

#else

/**
 * @ingroup c_interface
 * 
 * A handle for a grid
 */
typedef struct asagi_grid asagi_grid;

/**
 * @ingroup c_interface
 *
 * @see asagi::Grid::Type
 */
typedef enum { ASAGI_BYTE, ASAGI_INT, ASAGI_LONG, ASAGI_FLOAT, ASAGI_DOUBLE } asagi_type;
/**
 * @ingroup c_interface
 *
 * @see asagi::Grid::Error
 */
typedef enum {
	ASAGI_SUCCESS = 0,
	ASAGI_MPI_ERROR,
	ASAGI_THREAD_ERROR,
	ASAGI_NUMA_ERROR,
	ASAGI_UNKNOWN_PARAM,
	ASAGI_INVALID_VALUE,
	ASAGI_NOT_OPEN,
	ASAGI_VAR_NOT_FOUND,
	ASAGI_WRONG_SIZE,
	ASAGI_UNSUPPORTED_DIMENSIONS,
	ASAGI_MULTIPLE_TOPGRIDS,
	ASAGI_INVALID_VAR_SIZE
} asagi_error;
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @ingroup c_interface
 * 
 * @see asagi::Grid::create()
 */
asagi_grid* asagi_grid_create(asagi_type type);

/**
 * @ingroup c_interface
 * 
 * @see asagi::Grid::createArray()
 */
asagi_grid* asagi_grid_create_array(asagi_type basic_type);

/**
 * @ingroup c_interface
 * 
 * @see asagi::Grid::createStruct()
 */
asagi_grid* asagi_grid_create_struct(unsigned int count,
	unsigned int blockLength[],
	unsigned long displacements[],
	asagi_type types[]);

#ifndef ASAGI_NOMPI
/**
 * @ingroup c_interface
 * 
 * @see asagi::Grid::setComm()
 */
void asagi_grid_set_comm(asagi_grid* handle, MPI_Comm comm);
#endif
/**
 * @ingroup c_interface
 * 
 * @see asagi::Grid::setParam()
 */
void asagi_grid_set_param(asagi_grid* handle, const char* name,
	const char* value, unsigned int level);
/**
 * @ingroup c_interface
 * 
 * @see asagi::Grid::open()
 */
asagi_error asagi_grid_open(asagi_grid* handle, const char* filename,
	unsigned int level);

/**
 * @ingroup c_interface
 * 
 * @see asagi::Grid::getMin()
 */
double asagi_grid_min(asagi_grid* handle, unsigned int n);
/**
 * @ingroup c_interface
 *
 * @see asagi::Grid::getMax()
 */
double asagi_grid_max(asagi_grid* handle, unsigned int n);

/**
 * @ingroup c_interface
 * 
 * @see asagi::Grid::getDelta()
 */
double asagi_grid_delta(asagi_grid* handle, unsigned int n,
		unsigned int level);

/**
 * @ingroup c_interface
 *
 * @see asagi::Grid::getVarSize()
 */
unsigned int asagi_grid_var_size(asagi_grid* handle);

/**
 * @ingroup c_interface
 * 
 * @see asagi::Grid::getByte()
 */
unsigned char asagi_grid_get_byte(asagi_grid* handle, const double* pos,
	unsigned int level);
/**
 * @ingroup c_interface
 * 
 * @see asagi::Grid::getInt()
 */
int asagi_grid_get_int(asagi_grid* handle, const double* pos,
		unsigned int level);
/**
 * @ingroup c_interface
 * 
 * @see asagi::Grid::getLong()
 */
long asagi_grid_get_long(asagi_grid* handle, const double* pos,
		unsigned int level);
/**
 * @ingroup c_interface
 * 
 * @see asagi::Grid::getFloat()
 */
float asagi_grid_get_float(asagi_grid* handle, const double* pos,
		unsigned int level);
/**
 * @ingroup c_interface
 * 
 * @see asagi::Grid::getDouble()
 */
double asagi_grid_get_double(asagi_grid* handle, const double* pos,
		unsigned int level);
/**
 * @ingroup c_interface
 * 
 * @see asagi::Grid::getBuf()
 */
void asagi_grid_get_buf(asagi_grid* handle,void* buf, const double* pos,
	unsigned int level);

/**
 * @ingroup c_interface
 * 
 * @see asagi::Grid::close(asagi::Grid*)
 */
void asagi_grid_close(asagi_grid* handle);

#ifdef __cplusplus
}
#endif

#endif
