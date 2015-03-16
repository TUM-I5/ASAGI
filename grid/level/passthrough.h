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
 * @copyright 2013-2015 Sebastian Rettenberger <rettenbs@in.tum.de>
 */

#ifndef GRID_LEVEL_PASSTHROUGH_H
#define GRID_LEVEL_PASSTHROUGH_H

#include "level.h"
#include "types/type.h"

namespace grid
{

namespace level
{

/**
 * A simple grid that passes every access directly to the underlying
 * I/O layer.
 */
class PassThrough : public Level
{
private:
	/** Buffer for reading one value (on each thread) into memory */
	unsigned char* m_buf;

public:
	PassThrough();
	virtual ~PassThrough();

	asagi::Grid::Error init(
			const mpi::MPIComm &comm,
			const numa::Numa &numa,
			const types::Type *type,
			const char* filename,
			const char* varname,
			grid::ValuePosition valuePos);

	void getAt(void* buf, const double* pos,
			types::Type::converter_t converter)
	{
		incCounter(perf::Counter::ACCESS);
		incCounter(perf::Counter::FILE);

		// Load one value in each dimension
		size_t index[MAX_DIMENSIONS];
		pos2index(pos, index);
		size_t size[MAX_DIMENSIONS];
		std::fill_n(size, MAX_DIMENSIONS, 1);


		type()->load(inputFile(), index, size, m_buf);

		(type()->*converter)(m_buf, buf);
	}

#if 0
	/**
	 * @see Grid::keepFileOpen()
	 */
	virtual bool keepFileOpen() const
	{
		return true;
	}
#endif
};

}

}

#endif /* GRID_LEVEL_PASSTHROUGH_H */
