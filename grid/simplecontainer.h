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
 */

#ifndef GRID_SIMPLECONTAINER_H
#define GRID_SIMPLECONTAINER_H

#include <vector>

#include "typedcontainer.h"

namespace grid
{

/**
 * Simple container that stores the whole grid for each level.
 */
template<class Level, class MPIComm, class NumaComm, class Type>
class SimpleContainer : public TypedContainer<Type>
{
private:
	/** All grids we control */
	std::vector<Level> m_levels;

public:
	SimpleContainer(const mpi::MPIComm &comm,
			const numa::Numa &numa,
			Type &type,
			int timeDimension,
			ValuePosition valuePos)
		: TypedContainer<Type>(comm, numa, type, timeDimension, valuePos)
	{
	}

	virtual ~SimpleContainer()
	{
		m_levels.clear();
	}
	
	asagi::Grid::Error init(const char* filename,
			const char* varname,
			const unsigned int* blockSize,
			unsigned int cacheSize,
			int cacheHandSpread,
			unsigned int level)
	{
		if (m_levels.size() <= level)
			m_levels.resize(level+1,
					Level(this->comm(), this->numa(), this->type()));

		asagi::Grid::Error err = m_levels[level].open(
				filename,
				varname,
				blockSize,
				this->timeDimension(),
				cacheSize,
				cacheHandSpread,
				this->valuePosition());
		if (err != asagi::Grid::SUCCESS)
			return err;

		if (level == 0) {
			// Set min/max/...
			for (unsigned int i = 0; i < m_levels[0].dimensions(); i++) {
				this->m_min[i] = m_levels[0].min(i);
				this->m_max[i] = m_levels[0].max(i);
			}
		}

		return asagi::Grid::SUCCESS;
	}

	double getDelta(unsigned int n, unsigned int level) const
	{
		assert(n < MAX_DIMENSIONS);
		assert(level < m_levels.size());

		return m_levels[level].delta(n);
	}

	CONTAINER_GETVAR

	template<typename T>
	void getAt(T* buf, const double* pos, unsigned int level = 0)
	{
		assert(level < m_levels.size());

		m_levels[level].getAt(buf, pos);
	}
	
	double getXDelta() const;
	double getYDelta() const;
	double getZDelta() const;

	unsigned long getCounter(perf::Counter::CounterType type,
			unsigned int level = 0) const
	{
		assert(level < m_levels.size());

		return m_levels[level].getCounter(type);
	}
};

}

#endif // GRID_SIMPLECONTAINER_H
