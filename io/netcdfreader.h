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
		const size_t *size,
		unsigned int internalSize)
	{
		// Convert to char, so we can do pointer arithmetic
		unsigned char* const buffer = static_cast<unsigned char*>(block);

		switch (m_dimensions) {
		case 1:
			get1DimBlock<T>(buffer, offset, size);
			break;
		default:
			getNDimBlock<T>(buffer, offset, size, internalSize);
			break;
		}
		/*switch (m_dimensions) {
		case 1:
			if (xoffset + xsize > getXDim())
				xsize = getXDim() - xoffset;
			
			start[0] = xoffset;
			
			count[0] = xsize;
			break;
		case 2:
			if (yoffset + ysize > getYDim())
				ysize = getYDim() - yoffset;
		
			start[0] = yoffset;
			start[1] = xoffset;
		
			count[0] = ysize;
			count[1] = xsize;
		
			if (xoffset + xsize > getXDim()) {
				// we have to load data row by row
			
				count[0] = 1;
				count[1] = getXDim() - xoffset;
			
				for (size_t i = 0; i < ysize; i++) {
					m_variable.getVar(start, count,
						reinterpret_cast<T*>(&buffer[i * xsize
							* getVarSize()]));
				
					// Add 1 to the yoffset
					start[0]++;
				}
			
				return;
			}
			
			break;
		case 3:
			if (zoffset + zsize > getZDim())
				zsize = getZDim() - zoffset;
			
			start[0] = zoffset;
			start[1] = yoffset;
			start[2] = xoffset;
			
			count[0] = zsize;
			count[1] = ysize;
			count[2] = xsize;
			
			if (xoffset + xsize > getXDim()) {
				// we have to load data row by row
				
				count[0] = 1;
				count[1] = 1;
				count[2] = getXDim() - xoffset;
				
				y = ysize;
				if (yoffset + ysize > getYDim())
					y = getYDim() - yoffset;
				
				for (size_t i = 0; i < zsize; i++) {
					// loop through z dimension
					for (size_t j = 0; j < y; j++) {
						// loop through y dimension
						m_variable.getVar(start, count,
							reinterpret_cast<T*>(&buffer[(i * ysize + j) * xsize
								* getVarSize()]));
						
						// Add 1 to yoffset
						start[1]++;
					}
					
					// Add 1 to zoffset
					start[0]++;
					start[1] = yoffset;
				}
				return;
			}
			
			if (yoffset + ysize > getYDim()) {
				// we can do this slice by slice at least
				
				count[0] = 1;
				count[1] = getYDim() - yoffset;
				
				for (size_t i = 0; i < zsize; i++) {
					m_variable.getVar(start, count,
						reinterpret_cast<T*>(&buffer[i * ysize * xsize
							* getVarSize()]));
					
					// Add 1 to zoffset
					start[0]++;
				}
				
				return;
			}
			
			break;
		default:
			assert(false);
		}
		
		m_variable.getVar(start, count, static_cast<T*>(var));*/
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
	 * @copybrief getBlock
	 *
	 * This function works for 1D.
	 *
	 * @see getBlock
	 */
	template<typename T>
	void get1DimBlock(unsigned char *buffer,
		const size_t *offset,
		const size_t *size)
	{
		// The offset in the netcdf file (always same as offset)
		std::vector<size_t> actOffset(offset, offset+1);
		// Number of elements we read from the netcdf file
		std::vector<size_t> actSize(1);

		if (offset[0] + size[0] > getSize(0))
			actSize[0] = getSize(0) - offset[0];
		else
			actSize[0] = size[0];

		m_variable.getVar(actOffset, actSize,
			reinterpret_cast<T*>(buffer));


	}

	/**
	 * @copybrief getBlock
	 *
	 * This function works for arbitrary dimensions.
	 * 
	 * @param revOffset Offset ordered from x to z (reverse compared to the
	 *  order, the netcdf library needs)
	 * @param revSize Size ordered from x to z (same order as revOffset)
	 *
	 * @see getBlock
	 */
	template<typename T>
	void getNDimBlock(unsigned char *buffer,
		const size_t *revOffset,
		const size_t *revSize,
		unsigned int internalSize)
	{
		/*
		 * Original values (ordered x, y, z, ...)
		 * revOffset The offset of the block
		 * revSize   The size of the block, the caller wants
		 *
		 * Local values (ordered z, y, x, ...)
		 * offset    The offset of the block
		 * size      The size of the block, the caller wants
		 * actOffset The actual offset we use for the next netcdf call
		 * actSize   The number of elements we read from netcdf in one call
		 * maxSize   The number of elements we have to read in each direction
		 */

		std::vector<size_t> offset;
		offset.resize(m_dimensions);
		reverse_copy(revOffset, revOffset + m_dimensions, offset.begin());
		std::vector<size_t> size;
		size.resize(m_dimensions);
		reverse_copy(revSize, revSize + m_dimensions, size.begin());
		std::vector<size_t> actOffset(offset.begin(), offset.end());
		std::vector<size_t> actSize(size.begin(), size.end());
		std::vector<size_t> maxSize(size.begin(), size.end());

		for (int i = m_dimensions-1; i >= 0; i--) {
			if (offset[i] + size[i] > getSize(m_dimensions-i-1)) {
				// We do not have enough elements left in
				// dimension i
				actSize[i] = getSize(m_dimensions-i-1) - offset[i];
				maxSize[i] = actSize[i];
				
				// For all dimension before i, this means we
				// can only load 1 row add a time
				for (int j = 0; j < i; j++) {
					actSize[j] = 1;
					maxSize[j] = std::min(size[j],
						static_cast<size_t>(getSize(m_dimensions-j-1))-offset[j]);
				}

				break;
			}
		}

		// totalSize[i] = product(size[j]) for (j in [i:n-2])
		// -> The offset between to elements in dimension i is totalSize[i]
		std::vector<size_t> totalSize(m_dimensions);
		totalSize.back() = 1;
		for (int i = m_dimensions-2; i >= 0; i--)
			totalSize[i] = size[i+1] * totalSize[i+1];

		size_t bOffset = 0; // offset in the buffer
		while (actOffset[0] < offset[0] + maxSize[0]) {
			m_variable.getVar(actOffset, actSize,
				reinterpret_cast<T*>(&buffer[bOffset * internalSize]));
			
			actOffset.back() += actSize.back();
			bOffset += actSize.back();
			
			int i;
			for (i = m_dimensions-1; i > 0; i--) {
				if (actOffset[i] < offset[i] + maxSize[i])
					break;
				
				// Reset current dimension
				actOffset[i] = offset[i];
				bOffset -= maxSize[i] * totalSize[i];

				// Increment next dimension
				actOffset[i-1] += actSize[i-1];
				bOffset += actSize[i-1] * totalSize[i-1];
			}
		}
	}
};

}

#endif // IO_NETCDF_H
