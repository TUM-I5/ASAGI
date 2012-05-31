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
 */

#ifndef IO_NETCDF_H
#define IO_NETCDF_H

#include <asagi.h>

#include <cassert>
#include <limits>
#include <vector>
#include <netcdf>
#include <debug/dbg.h>

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
	
	/** Number of dimension (1, 2 or 3) */
	int m_dimensions;
	
	/** The name of the x dimension in the netcdf file */
	std::string m_nameX;
	/** The name of the y dimension in the netcdf file */
	std::string m_nameY;
	/** The name of the z dimension in the netcdf file */
	std::string m_nameZ;
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
	 * @return The size of the grid in x direction
	 */
	unsigned long getXDim() const
	{
		return m_file->getDim(m_nameX).getSize();
	}
	/**
	 * @return The size of the grid in y direction
	 */
	unsigned long getYDim() const
	{
		if (m_dimensions < 2)
			return 1;
		return m_file->getDim(m_nameY).getSize();
	}
	/**
	 * @return The size of the grid in z direction
	 */
	unsigned long getZDim() const
	{
		if (m_dimensions < 3)
			return 1;
		return m_file->getDim(m_nameZ).getSize();
	}
	
	/**
	 * @return The offset of the grid in x direction
	 */
	double getXOffset() const
	{
		double result;
		netCDF::NcVar x = m_file->getVar(m_nameX);
	
		if (x.isNull())
			return 0;
	
		x.getVar(std::vector<size_t>(1, 0), &result);
		return result;
	}
	/**
	 * @return The offset of the grid in y direction
	 */
	double getYOffset() const
	{
		double result;
		netCDF::NcVar y = m_file->getVar(m_nameY);
	
		if (y.isNull())
			return 0;
	
		y.getVar(std::vector<size_t>(1, 0), &result);
		return result;
	}
	/**
	 * @return The offset of the grid in z direction
	 */
	double getZOffset() const
	{
		double result;
		netCDF::NcVar z = m_file->getVar(m_nameZ);
	
		if (z.isNull())
			return 0;
	
		z.getVar(std::vector<size_t>(1, 0), &result);
		return result;
	}
	
	/**
	 * @return The scaling (the real distance between two cells)
	 *  in x direction
	 */
	double getXScaling() const
	{
		double first, last;
		std::vector<size_t> index(1);
		netCDF::NcVar x;
		unsigned long dim;
	
		dim = getXDim();
		if (dim == 1)
			return std::numeric_limits<double>::infinity();
		
		x = m_file->getVar(m_nameX);
		if (x.isNull())
			return 1;
	
		index[0] = 0;
		x.getVar(index, &first);
		index[0] = dim - 1;
		x.getVar(index, &last);
	
		return (last - first) / (dim - 1);
	}
	/**
	 * @return The scaling (the real distance between two cells)
	 *  in y direction
	 */
	double getYScaling() const
	{
		double first, last;
		std::vector<size_t> index(1);
		netCDF::NcVar y;
		unsigned long dim;
	
		if (m_dimensions < 2)
			return 0.;
		
		dim = getYDim();
	
		if (dim == 1)
			return std::numeric_limits<double>::infinity();
	
		y = m_file->getVar(m_nameY);
		if (y.isNull())
			return 1;
	
		index[0] = 0;
		y.getVar(index, &first);
		index[0] = dim - 1;
		y.getVar(index, &last);
	
		return (last - first) / (dim - 1);
	}
	/**
	 * @return The scaling (the real distance between two cells)
	 *  in y direction
	 */
	double getZScaling() const
	{
		double first, last;
		std::vector<size_t> index(1);
		netCDF::NcVar z;
		unsigned long dim;
	
		if (m_dimensions < 3)
			return 0.;
	
		dim = getZDim();
		if (dim == 1)
			return std::numeric_limits<double>::infinity();
	
		z = m_file->getVar(m_nameZ);
		if (z.isNull())
			return 1;
	
		index[0] = 0;
		z.getVar(index, &first);
		index[0] = dim - 1;
		z.getVar(index, &last);
	
		return (last - first) / (dim - 1);
	}
	
	/**
	 * Reads a block of cells from the netcdf file. The type is converted to
	 * T.
	 * 
	 * @param var The buffer where the values are written to
	 * @param xoffset Offset in x direction
	 * @param yoffset Offset in y direction
	 * @param zoffset Offset in z direction
	 * @param xsize Size of the block in x direction
	 * @param ysize Size of the block in y direction
	 * @param zsize Size of the block in z direction
	 */
	template<typename T> void getVar(void *var,
		size_t xoffset = 0, size_t yoffset = 0, size_t zoffset = 0,
		size_t xsize = 0, size_t ysize = 0, size_t zsize = 0)
	{
		size_t y;
		std::vector<size_t> start(m_dimensions);
		std::vector<size_t> count(m_dimensions);
		
		// Convert to char, so we can do pointer arthmetic
		unsigned char* const buffer = static_cast<unsigned char*>(var);
		
		if (xsize == 0)
			xsize = getXDim();
		
		if (ysize == 0)
			ysize = getYDim();
		
		if (zsize == 0)
			zsize = getZDim();
		
		switch (m_dimensions) {
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
		
		m_variable.getVar(start, count, static_cast<T*>(var));
	}
	
	/**
	 * @return The size of one cell in bytes
	 */
	unsigned int getVarSize() const
	{
		return m_variable.getType().getSize();
	}
};

}

#endif // IO_NETCDF_H