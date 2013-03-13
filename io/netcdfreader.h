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
 * @copyright 2012-2013 Sebastian Rettenberger <rettenbs@in.tum.de>
 */

#ifndef IO_NETCDF_H
#define IO_NETCDF_H

#include <asagi.h>

#include "grid/constants.h"

#include <cassert>
#include <limits>
#include <vector>
#include <algorithm>
#include <netcdf.h>

#include "debug/dbg.h"

/**
 * @brief Classes for read/writing files
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

	/** Number of dimension in the netCDF file */
	int m_dimensions;
	
	/** The name of the dimensions in the netCDF file */
	std::vector<std::string> m_names;

	/** The size of the dimensions in the netCDF file */
	std::vector<size_t> m_size;
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
	int getDimensions() const
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
		if (i >= getDimensions())
			return 0;

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
	 * @param offset Offset of the block (at least {@link getDimensions() values}
	 *  values)
	 * @param size Size of the block (at least {@link getDimensions()} values)
	 */
	template<typename T>
	void getBlock(void *block,
		const size_t *offset,
		const size_t *size)
	{
		// The netCDF standard (Coords convention) uses Fortran
		// dimension ordering -> reverse offset and size arrays
		size_t actOffset[m_dimensions];
		std::reverse_copy(offset, offset+m_dimensions, actOffset);
		size_t actSize[m_dimensions];
		std::reverse_copy(size, size+m_dimensions, actSize);

		// Make sure we do not read more than the available data
		// in each dimension
		for (int i = 0; i < m_dimensions; i++) {
			if (actOffset[i] + actSize[i] > getSize(m_dimensions-i-1))
				actSize[i] = getSize(m_dimensions-i-1) - actOffset[i];
		}

		// The distance between 2 values in the internal representation
		ptrdiff_t imap[m_dimensions];
		imap[m_dimensions-1] = getBasicSize<T>();
		for (int i = m_dimensions-2; i >= 0; i--)
			imap[i] = imap[i+1] * size[m_dimensions-i-1];

		getVar(actOffset, actSize, imap,
			static_cast<T*>(block));
	}
	
	/**
	 * @return The size of one cell in bytes
	 */
	unsigned int getVarSize() const
	{
		nc_type type;
		nc_inq_vartype(m_file, m_variable, &type);
		size_t size;
		nc_inq_type(m_file, type, 0L, &size);

		return size;
	}
	
private:
	/**
	 * @return The size of one value in a hyperslab
	 */
	template<typename T>
	size_t getBasicSize()
	{
		return 1;
	}

	/**
	 * Reads a hyperslab from the netCDF file
	 */
	template<typename T>
	void getVar(const size_t *offset, const size_t *size,
		const ptrdiff_t *imap, T *buffer);

private:
	static const ptrdiff_t STRIDE[grid::MAX_DIMENSIONS];
};

template<> inline
size_t NetCdfReader::getBasicSize<void>()
{
	return getVarSize();
}

template<> inline
void NetCdfReader::getVar(const size_t *offset, const size_t *size,
		const ptrdiff_t *imap, unsigned char *buffer)
{
	nc_get_varm_uchar(m_file, m_variable, offset, size, STRIDE, imap, buffer);
}

template<> inline
void NetCdfReader::getVar(const size_t *offset, const size_t *size,
		const ptrdiff_t *imap, int *buffer)
{
	nc_get_varm_int(m_file, m_variable, offset, size, STRIDE, imap, buffer);
}

template<> inline
void NetCdfReader::getVar(const size_t *offset, const size_t *size,
		const ptrdiff_t *imap, long *buffer)
{
	nc_get_varm_long(m_file, m_variable, offset, size, STRIDE, imap, buffer);
}

template<> inline
void NetCdfReader::getVar(const size_t *offset, const size_t *size,
		const ptrdiff_t *imap, float *buffer)
{
	nc_get_varm_float(m_file, m_variable, offset, size, STRIDE, imap, buffer);
}

template<> inline
void NetCdfReader::getVar(const size_t *offset, const size_t *size,
		const ptrdiff_t *imap, double *buffer)
{
	nc_get_varm_double(m_file, m_variable, offset, size, STRIDE, imap, buffer);
}

template<> inline
void NetCdfReader::getVar(const size_t *offset, const size_t *size,
		const ptrdiff_t *imap, void *buffer)
{
	nc_get_varm(m_file, m_variable, offset, size, STRIDE, imap, buffer);
}

}

#endif // IO_NETCDF_H
