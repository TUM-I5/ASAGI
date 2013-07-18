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

#include "netcdfreader.h"

#define DIM_NOT_MAPPED "<not mapped>"

/**
 * @param filename The name of the netcdf file
 * @param rank The rank of this MPI process
 */
io::NetCdfReader::NetCdfReader(const char* filename, int rank)
	: m_filename(filename),
	m_rank(rank)
{
	m_file = -1;
}

io::NetCdfReader::~NetCdfReader()
{
	if (m_file >= 0)
		nc_close(m_file);
}

/**
 * Opens the netcdf file and reads the header information
 * 
 * @param varname The name of the netcdf variable that should be read
 *  later
 */
asagi::Grid::Error io::NetCdfReader::open(const char* varname)
{
	if (nc_open(m_filename.c_str(), NC_NOWRITE, &m_file) != NC_NOERR) {
		// Could not open file
		
		m_file = -1;
		return asagi::Grid::NOT_OPEN;
	}
	
	if (nc_inq_varid(m_file, varname, &m_variable) != NC_NOERR) {
		
		nc_close(m_file);
		m_file = -1;
		return asagi::Grid::VAR_NOT_FOUND;
	}

	nc_inq_varndims(m_file, m_variable, &m_dimensions);
	
	if (m_dimensions > grid::MAX_DIMENSIONS) {
		dbgDebug(m_rank) << "Unsupported number of variable dimensions:"
			<< m_dimensions;

		nc_close(m_file);
		m_file = -1;
		return asagi::Grid::UNSUPPORTED_DIMENSIONS;
	}

	dbgDebug(m_rank) << "Dimension mapping:";
	int dimIds[grid::MAX_DIMENSIONS];
	nc_inq_vardimid(m_file, m_variable, dimIds);
	for (int i = 0; i < m_dimensions; i++) {
		// Translates dimension order from Fortran to C/C++
		char name[NC_MAX_NAME+1];
		size_t size;

		nc_inq_dim(m_file, dimIds[m_dimensions - i - 1], name, &size);
		m_names.push_back(name);
		m_size.push_back(size);
		
		dbgDebug(m_rank) << "\t" << grid::DIMENSION_NAMES[i] << ":="
			<< m_names[i];
	}
	
	return asagi::Grid::SUCCESS;
}

/**
 * The stride is always 1 in each dimension, thus we define the stride array only once.
 */
const ptrdiff_t io::NetCdfReader::STRIDE[grid::MAX_DIMENSIONS] = {1, 1, 1};
