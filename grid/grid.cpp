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
 * @copyright 2015 Sebastian Rettenberger <rettenbs@in.tum.de>
 */

#include "grid.h"

#include <algorithm>

#include "utils/stringutils.h"

#include "simplecontainer.h"
#include "level/passthrough.h"
#include "types/arraytype.h"
#include "types/basictype.h"
#include "types/structtype.h"

/**
 * @param type The basic type of the values
 * @param isArray True if the type is an array, false if it is a basic
 *  type
 */
grid::Grid::Grid(asagi::Grid::Type type, bool isArray)
{
	init();

	if (isArray) {
		switch (type) {
		case BYTE:
			m_type = new types::ArrayType<unsigned char>();
			break;
		case INT:
			m_type = new types::ArrayType<int>();
			break;
		case LONG:
			m_type = new types::ArrayType<long>();
			break;
		case FLOAT:
			m_type = new types::ArrayType<float>();
			break;
		case DOUBLE:
			m_type = new types::ArrayType<double>();
			break;
		default:
			m_type = 0L;
			assert(false);
			break;
		}
	} else {
		switch (type) {
		case BYTE:
			m_type = new types::BasicType<unsigned char>();
			break;
		case INT:
			m_type = new types::BasicType<int>();
			break;
		case LONG:
			m_type = new types::BasicType<long>();
			break;
		case FLOAT:
			m_type = new types::BasicType<float>();
			break;
		case DOUBLE:
			m_type = new types::BasicType<double>();
			break;
		default:
			m_type = 0L;
			assert(false);
			break;
		}
	}
}

/**
 * @param count Number of elements in the struct
 * @param blockLength Size of each element in the struct
 * @param displacements Offset of each element in the struct
 * @param types Type of each element in the struct
 */
grid::Grid::Grid(unsigned int count,
		unsigned int blockLength[],
		unsigned long displacements[],
		asagi::Grid::Type types[])
{
	init();

	m_type = types::createStruct(count, blockLength,
		displacements, types);
}


grid::Grid::~Grid()
{
	delete m_type;

	for (std::vector<Container*>::const_iterator it = m_containers.begin();
			it != m_containers.end(); it++)
		delete *it;

	// Remove from fortran <-> c translation
	m_pointers.remove(m_id);
}

void grid::Grid::setParam(const char* name, const char* value, unsigned int level)
{
	if (m_params.size() <= level)
		m_params.resize(level+1);

	// Convert everything to uppercase
	std::string n = name;
	utils::StringUtils::toUpper(n);
	std::replace(n.begin(), n.end(), '-', '_');

	std::string v = value;
	utils::StringUtils::toUpper(v);
	std::replace(v.begin(), v.end(), '-', '_');

	m_params[level][n] = v;
}

asagi::Grid::Error grid::Grid::open(const char* filename, unsigned int level)
{
	bool domainMaster;
	asagi::Grid::Error err = m_numa.registerThread(domainMaster);
	if (err != asagi::Grid::SUCCESS)
		return err;

	if (domainMaster) {
		// Make sure the container has the correct size
		m_resizeOnce.saveExec(*this, &Grid::initContainers);

		return m_containers[m_numa.domainId()]->init(filename,
				param("VARIABLE", "z"),
				level);
	}

	return asagi::Grid::SUCCESS;
}

unsigned long grid::Grid::getCounter(const char* name, unsigned int level)
{
	perf::Counter::CounterType type = perf::Counter::name2type(name);

	unsigned long counter = 0;

	// Aggregate over all NUMA domains
	for (std::vector<Container*>::const_iterator it = m_containers.begin();
			it != m_containers.end(); it++)
		counter += (*it)->getCounter(type);

	return counter;
}

/**
 * Initialize basic variables
 * Should only be called once by the constructor
 */
void grid::Grid::init()
{
	// Prepare for fortran <-> c translation
	m_id = m_pointers.add(this);
}

/**
 * Initializes all the containers
 */
void grid::Grid::initContainers()
{
	enum {
		PASS_THROUGH,
		UNKNOWN
	} containerType = UNKNOWN;

	// Select the container type
	if (param("PASS_THROUGH", false)) {
		containerType = PASS_THROUGH;
	}

	// Value position
	std::string strValuePos = param("VALUE_POSITION", "CELL_CENTERED");
	ValuePosition valuePos;
	if (strValuePos == "VERTEX_CENTERED")
		valuePos = VERTEX_CENTERED;
	else {
		valuePos = CELL_CENTERED;
		if (strValuePos != "CELL_CENTERED")
			logWarning(m_comm.rank()) << "ASAGI: Unknown value position:" << strValuePos;
	}

	// Initialize the container
	m_containers.resize(m_numa.totalDomains());
	for (std::vector<Container*>::iterator it = m_containers.begin();
			it != m_containers.end(); it++) {
		switch (containerType) {
		case PASS_THROUGH:
			*it = new SimpleContainer<level::PassThrough>(m_comm, m_numa, m_type,
					valuePos);
			break;
		default:
			*it = 0L;
			assert(false);
		}
	}
}

/**
 * Checks if a parameter exists and returns its value.
 * If the parameter is not set, the default value is returned.
 */
template<typename T>
T grid::Grid::param(const char* name, T defaultValue, unsigned int level) const
{
	if (level >= m_params.size())
		return defaultValue;

	std::map<std::string, std::string>::const_iterator it = m_params[level].find(name);
	if (it == m_params[level].end())
		return defaultValue;

	return utils::StringUtils::parse<T>(it->second, true);
}

// Fortran <-> c translation array
fortran::PointerArray<grid::Grid>
	grid::Grid::m_pointers;
