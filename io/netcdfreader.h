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
 */

#ifndef IO_NETCDF_H
#define IO_NETCDF_H

#include <asagi.h>

#include <cassert>
#include <limits>
#include <vector>
#include <algorithm>
#include <netcdf>

#include "gridconstants.h"
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
	
	/** the file, this class will work on */
	const netCDF::NcFile* m_file;
	
	/** The variable we read */
	netCDF::NcVar m_variable;

	/** Number of dimension in the netCDF file */
	int m_dimensions;
	
	/** The name of the dimensions in the netcdf file */
	std::vector<std::string> m_names;
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
		return m_file != 0L;
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
		return m_file->getDim(m_names[i]).getSize();
	}
	
	/**
	 * @return The offset of the grid in i-th direction
	 */
	double getOffset(int i) const
	{
		if (i >= getDimensions())
			return 0;

		netCDF::NcVar x = m_file->getVar(m_names[i]);
	
		if (x.isNull())
			return 0;

		double result;
		x.getVar(std::vector<size_t>(1, 0), &result);
		return result;
	}
	
	/**
	 * @return The scaling (the real distance between two cells)
	 *  in i-th direction
	 */
	double getScaling(int i) const
	{
		double first, last;
		std::vector<size_t> index(1);
		netCDF::NcVar x;
	
		if (i >= m_dimensions)
			return 0.;
	
		unsigned long size = getSize(i);
		if (size == 1)
			return std::numeric_limits<double>::infinity();
		
		x = m_file->getVar(m_names[i]);
		if (x.isNull())
			return 1;
	
		index[0] = 0;
		x.getVar(index, &first);
		index[0] = size - 1;
		x.getVar(index, &last);
	
		return (last - first) / (size - 1);
	}
	
	/**
	 * \brief Reads a block of cells from the netcdf file. The type is converted
	 *  to T.
	 * 
	 * @param block The buffer where the values are written to
	 * @param offset Offset of the block (at least {@link getDim()}
	 *  values)
	 * @param size Size of the block (at least {@link getDim()} values)
	 * @param internalSize Number of bytes used for one value,
	 *  can be different from size in the netcdf file
	 */
	template<typename T>
	void getBlock(void *block,
		const size_t *offset,
		const size_t *size)
	{
		// Convert to char, so we can do pointer arithmetic
		unsigned char* const buffer = static_cast<unsigned char*>(block);

		// The netCDF standard (Coords convention) uses Fortran
		// dimension ordering -> reverse offset and size arrays
		std::vector<size_t> actOffset;
		actOffset.resize(m_dimensions);
		reverse_copy(offset, offset+m_dimensions, actOffset.begin());
		std::vector<size_t> actSize;
		actSize.resize(m_dimensions);
		reverse_copy(size, size+m_dimensions, actSize.begin());

		// Make sure we do not read moe than the available data
		// in each dimension
		for (int i = 0; i < m_dimensions; i++) {
			if (actOffset[i] + actSize[i] > getSize(m_dimensions-i-1))
				actSize[i] = getSize(m_dimensions-i-1) - actOffset[i];
		}

		// Stride is always 1
		std::vector<ptrdiff_t> stride(m_dimensions, 1);

		// The distance between 2 values in the internal representation
		std::vector<ptrdiff_t> imap(m_dimensions);
		imap[m_dimensions-1] = getBasicSize<T>();
		for (int i = m_dimensions-2; i >= 0; i--)
			imap[i] = imap[i+1] * size[m_dimensions-i-1];

		m_variable.getVar(actOffset, actSize, stride, imap,
				reinterpret_cast<T*>(buffer));
	}
	
	/**
	 * @return The size of one cell in bytes
	 */
	unsigned int getVarSize() const
	{
		return m_variable.getType().getSize();
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
};

template<> inline
size_t NetCdfReader::getBasicSize<void>()
{
	return getVarSize();
}

}

#endif // IO_NETCDF_H
