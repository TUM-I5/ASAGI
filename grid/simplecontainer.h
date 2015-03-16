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

#include "container.h"

namespace grid
{

/**
 * Simple container that stores the whole grid for each level.
 */
template<class Level>
class SimpleContainer : public Container
{
private:
	/** All grids we control */
	std::vector<Level> m_levels;

public:
	SimpleContainer(const mpi::MPIComm &comm,
			const numa::Numa &numa,
			const types::Type* type,
			ValuePosition valuePos)
		: Container(comm, numa, type, valuePos)
	{
	}

	virtual ~SimpleContainer()
	{ }
	
	asagi::Grid::Error init(const char* filename,
			const char* varname,
			unsigned int level)
	{
		if (m_levels.size() <= level)
			m_levels.resize(level+1);

		return m_levels[level].init(
				comm(),
				numa(),
				type(),
				filename,
				varname,
				valuePosition());
	}

	void getAt(void* buf, const double* pos,
			types::Type::converter_t converter, unsigned int level = 0)
	{
		assert(level < m_levels.size());

		m_levels[level].getAt(buf, pos, converter);
	}
	
	double getXDelta() const;
	double getYDelta() const;
	double getZDelta() const;

	unsigned char getByte3D(double x, double y = 0, double z = 0,
		unsigned int level = 0);
	int getInt3D(double x, double y = 0, double z = 0,
		unsigned int level = 0);
	long getLong3D(double x, double y = 0, double z = 0,
		unsigned int level = 0);
	float getFloat3D(double x, double y = 0, double z = 0,
		unsigned int level = 0);
	double getDouble3D(double x, double y = 0, double z = 0,
		unsigned int level = 0);
	void getBuf3D(void* buf, double x, double y = 0, double z = 0,
		unsigned int level = 0);

	unsigned long getCounter(perf::Counter::CounterType type,
			unsigned int level = 0) const
	{
		assert(level < m_levels.size());

		return m_levels[level].getCounter(type);
	}
};

}

#endif // GRID_SIMPLECONTAINER_H
