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
 * @copyright 2012-2016 Sebastian Rettenberger <rettenbs@in.tum.de>
 */

#ifndef IO_NETCDFREADER_H
#define IO_NETCDFREADER_H

#include "asagi.h"

#include <algorithm>
#include <cassert>
#include <cstring>
#include <limits>
#include <mutex>
#include <string>
#include <vector>

#ifndef ASAGI_NOMPI
#ifndef MPI_INCLUDED
#define MPI_INCLUDED
#define MPI_INCLUDED_NETCDF
#endif // MPI_INCLUDED
#endif // ASAGI_NOMPI
#include <netcdf.h>
#ifdef MPI_INCLUDED_NETCDF
#undef MPI_INCLUDED
#undef MPI_INCLUDED_NETCDF
#endif // MPI_INCLUDED_NETCDF

#include "threads/mutex.h"

/**
 * @brief Classes for read/writing files
 *
 * @todo Test if we can do better than netCDFs varm
 */
namespace io
{

/**
 * Provides access to <b>one</b> grid in a netCDF file
 */
class NetCdfReader
{
private:
	/** The name of the netcdf file */
	std::string m_filename;
	
	/** MPI rank */
	const int m_rank;
	
	/** The file (netCDF file id), this class will work on */
	int m_file;
	
	/** The variable we read */
	int m_variable;

	/** The size of the dimensions in the netCDF file */
	size_t m_varSize;

	/** Number of dimension in the netCDF file */
	int m_dimensions;
	
	/** The name of the dimensions in the netCDF file */
	std::string* m_names;

	/** The size of the dimensions in the netCDF file */
	size_t* m_size;

	// Must be declared before the functions
	static threads::Mutex netcdfLock;

public:
	NetCdfReader(const char* filename, int rank);
	virtual ~NetCdfReader();
	
	asagi::Grid::Error open(const char* varname = "z");
	
	/**
	 * @return <code>True</code> if the netcdf file is opened,
	 *  <code>false</code> otherwise
	 */
	bool isOpen() const
	{
		return m_file < 0;
	}
	
	/**
	 * @return The number of dimensions
	 */
	unsigned int dimensions() const
	{
		return m_dimensions;
	}
	
	/**
	 * @return The size of the grid in i-th direction
	 */
	unsigned long getSize(int i) const
	{
		if (i >= m_dimensions)
			return 1;
		return m_size[i];
	}
	
	/**
	 * @return The offset of the grid in i-th direction
	 */
	double getOffset(int i) const
	{
		if (i >= m_dimensions)
			return 0;

		std::lock_guard<threads::Mutex> lock(netcdfLock);
		int x;
		if (nc_inq_varid(m_file, m_names[i].c_str(), &x) != NC_NOERR)
			return 0;

		double result;
		size_t index = 0;
		nc_get_var1_double(m_file, x, &index, &result);
		return result;
	}
	
	/**
	 * @return The scaling (the real distance between two cells)
	 *  in i-th direction
	 */
	double getScaling(int i) const
	{
		if (i >= m_dimensions)
			return 0.;
	
		unsigned long size = getSize(i);
		if (size == 1)
			return std::numeric_limits<double>::infinity();

		std::lock_guard<threads::Mutex> lock(netcdfLock);
		int x;
		if (nc_inq_varid(m_file, m_names[i].c_str(), &x) != NC_NOERR)
			return 1;

		double first, last;
		size_t index = 0;
		nc_get_var1_double(m_file, x, &index, &first);
		index = size - 1;
		nc_get_var1_double(m_file, x, &index, &last);
	
		return (last - first) / (size - 1);
	}
	
	/**
	 * \brief Reads a block of cells from the netcdf file. The type is converted
	 *  to T.
	 * 
	 * @param block The buffer where the values are written to
	 * @param offset Offset of the block (at least {@link dimensions() values}
	 *  values)
	 * @param size Size of the block (at least {@link dimensions()} values)
	 */
	template<typename T>
	void getBlock(void *block,
		const size_t *offset,
		const size_t *size)
	{
		// Make sure we do not read more than the available data
		// in each dimension
		size_t actSize[MAX_DIMENSIONS];
		for (int i = 0; i < m_dimensions; i++) {
			if (offset[i] + size[i] > getSize(i))
				actSize[i] = getSize(i) - offset[i];
			else
				actSize[i] = size[i];
		}

		// The distance between 2 values in the internal representation
		ptrdiff_t imap[MAX_DIMENSIONS];
		imap[m_dimensions-1] = 1;
		for (int i = m_dimensions-2; i >= 0; i--)
			imap[i] = imap[i+1] * size[i+1];

		std::lock_guard<threads::Mutex> lock(netcdfLock);
		getVar(offset, actSize, size, imap,
			static_cast<T*>(block));
	}
	
	/**
	 * @return The size of one cell in bytes
	 */
	unsigned int getVarSize() const
	{
		return m_varSize;
	}
	
private:
	/**
	 * Reads a hyperslab from the netCDF file
	 *
	 * @param offset The offset in the netCDF file
	 * @param size The number of elements that should be red in each dimension
	 * @param blockSize The size of the memory block
	 * @param imap The netCDF mapping parameter
	 * @param buffer The buffer for the block
	 */
	template<typename T>
	void getVar(const size_t *offset, const size_t *size,
		const size_t *blockSize, const ptrdiff_t *imap, T *buffer);
};

/**
 * @copydoc getVar\<T\>
 */
template<> inline
void NetCdfReader::getVar(const size_t *offset, const size_t *size,
		const size_t *blockSize, const ptrdiff_t *imap, unsigned char *buffer)
{
	nc_get_varm_uchar(m_file, m_variable, offset, size, 0L, imap, buffer);
}

/**
 * @copydoc getVar\<T\>
 */
template<> inline
void NetCdfReader::getVar(const size_t *offset, const size_t *size,
		const size_t *blockSize, const ptrdiff_t *imap, int *buffer)
{
	nc_get_varm_int(m_file, m_variable, offset, size, 0L, imap, buffer);
}

/**
 * @copydoc getVar\<T\>
 */
template<> inline
void NetCdfReader::getVar(const size_t *offset, const size_t *size,
		const size_t *blockSize, const ptrdiff_t *imap, long *buffer)
{
	nc_get_varm_long(m_file, m_variable, offset, size, 0L, imap, buffer);
}

/**
 * @copydoc getVar\<T\>
 */
template<> inline
void NetCdfReader::getVar(const size_t *offset, const size_t *size,
		const size_t *blockSize, const ptrdiff_t *imap, float *buffer)
{
	nc_get_varm_float(m_file, m_variable, offset, size, 0L, imap, buffer);
}

/**
 * @copydoc getVar\<T\>
 */
template<> inline
void NetCdfReader::getVar(const size_t *offset, const size_t *size,
		const size_t *blockSize, const ptrdiff_t *imap, double *buffer)
{
	nc_get_varm_double(m_file, m_variable, offset, size, 0L, imap, buffer);
}

/**
 * @copydoc getVar\<T\>
 */
template<> inline
void NetCdfReader::getVar(const size_t *offset, const size_t *size,
		const size_t *blockSize, const ptrdiff_t *imap, void *buffer)
{
	// Read the data unmapped (netCDF does not support mapping for non-builtin data types)
	nc_get_vara(m_file, m_variable, offset, size, buffer);

	// Mapping required?
	bool doMapping = false;
	for (int i = 1; i < m_dimensions; i++)
		if (size[i] != blockSize[i])
			doMapping = true;

	if (!doMapping)
		return;

	// Apply mapping ...
	char* byteBuf = reinterpret_cast<char*>(buffer);

	// Compute initial indices and position
	long pos[MAX_DIMENSIONS];
	long indexFile = 1;
	for (int i = 0; i < m_dimensions; i++) {
		pos[i] = size[i]-1;
		indexFile *= size[i];
	}

	size_t indexBuf = size[0]-1;
	for (int i = 1; i < m_dimensions; i++) {
		indexBuf *= blockSize[i];
		indexBuf += size[i]-1;
	}

	// Loop over all elements
	for (indexFile--; indexFile >= 0 && indexFile < static_cast<long>(indexBuf); indexFile--) {
		memcpy(&byteBuf[indexBuf * getVarSize()],
				&byteBuf[indexFile * getVarSize()],
				getVarSize());

		// Adjust the actual position
		pos[m_dimensions-1]--;
		indexBuf--;

		for (int i = m_dimensions-1; pos[i] < 0 && i > 0; i--) {
			pos[i-1]--;
			pos[i] = size[i]-1;

			indexBuf -= imap[i-1];
			indexBuf += imap[i] * size[i];
		}
	}
}

}

#endif // IO_NETCDFREADER_H
