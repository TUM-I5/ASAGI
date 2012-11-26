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

#include "netcdfreader.h"

#include "gridconstants.h"

#include "debug/dbg.h"

#define DIM_NOT_MAPPED "<not mapped>"

using namespace asagi;

using namespace netCDF;
using namespace netCDF::exceptions;

/**
 * @param filename The name of the netcdf file
 * @param rank The rank of this MPI process
 */
io::NetCdfReader::NetCdfReader(const char* filename, int rank)
	: m_filename(filename),
	m_rank(rank)
{
	m_file = 0L;
}

io::NetCdfReader::~NetCdfReader()
{
	delete m_file;
}

/**
 * Opens the netcdf file and reads the header information
 * 
 * @param varname The name of the netcdf variable that should be read
 *  later
 */
Grid::Error io::NetCdfReader::open(const char* varname)
{
	try {
		m_file = new NcFile(m_filename.c_str(), NcFile::read);
	} catch (NcException& e) {
		// Could not open file
		
		m_file = 0L;
		return Grid::NOT_OPEN;
	}
	
	m_variable = m_file->getVar(varname);
		
	if (m_variable.isNull())
		return Grid::VAR_NOT_FOUND;

	m_dimensions = m_variable.getDimCount();
	
	if (m_dimensions > MAX_DIMENSIONS) {
		dbgDebug(m_rank) << "Unsupported number of variable dimensions:"
			<< m_dimensions;
		return Grid::UNSUPPORTED_DIMENSIONS;
	}
	
	dbgDebug(m_rank) << "Dimension mapping:";
	for (int i = 0; i < m_dimensions; i++) {
		// Translates dimension order from Fortran to C/C++
		m_names.push_back(m_variable.getDim(m_dimensions - i - 1)
			.getName());
		
		dbgDebug(m_rank) << "\t" << DIMENSION_NAMES[i] << ":="
			<< m_names[i];
	}
	
	return Grid::SUCCESS;
}
