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
		/** Function is not yet initialized */
		NOT_INITIALIZED,
		/** Function already initialized */
		ALREADY_INITIALIZED,
		/** Could not open input file */
		NOT_OPEN,
		/** netCDF variable not found */
		VAR_NOT_FOUND,
		/** Wrong variable size in the file */
		WRONG_SIZE,
		/** Unsupported number of dimensions input file */
		UNSUPPORTED_DIMENSIONS,
		/** Variable size in the input file does not match the type */
		INVALID_VAR_SIZE
	};
	
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
	 * See @ref Parameters for a list of supported parameters.
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
	 * @param n The dimension
	 * @return The minimum allowed coordinate in dimension <code>n</code>
	 */
	virtual double getMin(unsigned int n) const = 0;

	/**
	 * @ingroup cxx_interface
	 * 
	 * @param n The dimension
	 * @return The maximum allowed coordinate in dimension <code>n</code>
	 */
	virtual double getMax(unsigned int n) const = 0;
	
	/**
	 * @ingroup cxx_interface
	 * 
	 * @param n The dimension
	 * @param level The level for which the difference is requested
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
	 * @param level The level from which the data should be fetched
	 * @return The element at <code>pos</code> as a char
	 */
	virtual unsigned char getByte(const double* pos, unsigned int level = 0) = 0;
	/**
	 * @ingroup cxx_interface
	 * 
	 * @param pos The coordinates of the value, the array must have at least
	 *  the size of the dimension of the grid
	 * @param level The level from which the data should be fetched
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
	 * @param level The level from which the data should be fetched
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
	 * @param level The level from which the data should be fetched
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
	 * @param level The level from which the data should be fetched
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
	 * @param buf Pointer to the buffer where the data should be written
	 * @param pos The coordinates of the value, the array must have at least
	 *  the size of the dimension of the grid
	 * @param level The level from which the data should be fetched
	 */
	virtual void getBuf(void* buf, const double* pos, unsigned int level = 0) = 0;

	/**
	 * @ingroup cxx_interface
	 *
	 * Gets the current value of a counter for a grid level.
	 *
	 * See @ref accesscounter for a list of all counters.
	 *
	 * @return The current counter value or 0 if the name is not defined
	 *
	 * @warning The performance counters are by default not thread-safe for
	 *  performance reason. You may get wrong result when using more than one
	 *  thread.
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
	 * Frees all memory resources associated with @c grid. After a grid is
	 * closed you cannot access any values and you can not reopen another
	 * NetCDF file.
	 * <br>
	 * This function does the same as calling <code>delete grid;</code> and
	 * it is the C++ equivalent to {@link asagi_grid_close(asagi_grid*)} and
	 * {@link asagi::asagi_grid_close}
	 * 
	 * @param grid The grid that should be closed.
	 */
	static void close(asagi::Grid* grid)
	{
		delete grid;
	}

#ifndef ASAGI_NOMPI
	/**
	 * @ingroup cxx_interface
	 *
	 * Starts the communication thread. This must be done before a grid with the option
	 * <code>MPI_COMMUNICATION = THREAD</code> is opened.
	 *
	 * This is a collective operation within <code>comm</code>.
	 *
	 * @param schedCPU The id of the CPU on which the communication thread should run.
	 *  Use negative values to select the last, second last, ... CPU. If the id is
	 *  invalid, the thread will not be pinned to a CPU.
	 * @param comm The communicator specifying which processes will be involved in
	 *  any communication
	 * @return <code>SUCCESS</code> or an error
	 */
	static Error startCommThread(int schedCPU = -1, MPI_Comm comm = MPI_COMM_WORLD);

	/**
	 * @ingroup cxx_interface
	 *
	 * Stops the communication thread
	 */
	static void stopCommThread();

	/**
	 * @ingroup cxx_interface
	 *
	 * Computes the rank of the process on the node. This function can be used to
	 * determine the core, to which the communication threads should be pinned in
	 * {@link startCommThread}.
	 *
	 * @param comm The communicator that should be used
	 * @return The node local rank
	 */
	static int nodeLocalRank(MPI_Comm comm = MPI_COMM_WORLD);
#endif // ASAGI_NOMPI
};

typedef asagi::Grid asagi_grid;
/**
 * @see asagi::Grid::Type
 */
typedef asagi::Grid::Type asagi_type;
/**
 * @see asagi::Grid::Error
 */
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
	ASAGI_NOT_INITIALIZED,
	ASAGI_ALREADY_INITIALIZED,
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
 * @see asagi::Grid::setThreads()
 */
void asagi_grid_set_threads(asagi_grid* handle, unsigned int threads);

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

#ifndef ASAGI_NOMPI
/**
 * @ingroup c_interface
 *
 * @see asagi::Grid::startCommThread(int, MPI_Comm)
 */
asagi_error asagi_start_comm_thread(int sched_cpu, MPI_Comm comm);

/**
 * @ingroup c_interface
 *
 * @see asagi::Grid::stopCommThread()
 */
void asagi_stop_comm_thread();

/**
 * @ingroup c_interface
 *
 * @see asagi::Grid::nodeLocalRank(MPI_Comm)
 */
int asagi_node_local_rank(MPI_Comm comm);
#endif

#ifdef __cplusplus
}
#endif

#endif
