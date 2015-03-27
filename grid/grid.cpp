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
#include "magic/typelist.h"
#include "level/full.h"
#include "level/passthrough.h"
#include "types/arraytype.h"
#include "types/basictype.h"
#include "types/structtype.h"

/**
 * Creates the container depending on the type of the <code>grid</code>
 */
template<template<class Level, class MPIComm, class NumaComm, class Type> class Container,
	template<class MPIComm, class NumaComm, class Type> class Level,
	class MPIComm, class NumaComm, class TypeList>
grid::Container* grid::Grid::TypeSelector<Container, Level, MPIComm, NumaComm, TypeList>
	::createContainer(Grid &grid)
{
	typedef typename TypeList::Head Head;
	typedef typename TypeList::Tail Tail;

	if (Head* type = dynamic_cast<Head*>(grid.m_type)) {
		// Time dimension
		int timeDimension = grid.param("TIME_DIMENSION", -1);

		// Value position
		std::string strValuePos = grid.param("VALUE_POSITION", "CELL_CENTERED");
		ValuePosition valuePos;
		if (strValuePos == "VERTEX_CENTERED")
			valuePos = VERTEX_CENTERED;
		else {
			valuePos = CELL_CENTERED;
			if (strValuePos != "CELL_CENTERED") {
				logWarning(grid.m_comm.rank()) << "ASAGI: Unknown value position:" << strValuePos;
				logWarning(grid.m_comm.rank()) << "ASAGI: Assuming CELL_CENTERED";
			}
		}

		// Create the container
		return new Container<Level<MPIComm, NumaComm, Head>, MPIComm, NumaComm, Head>(grid.m_comm, grid.m_numa,
				*type, timeDimension, valuePos);
	}

	return TypeSelector<Container, Level, MPIComm, NumaComm, Tail>::createContainer(grid);
}

template<template<class Level, class MPIComm, class NumaComm, class Type> class Container,
	template<class MPIComm, class NumaComm, class Type> class Level,
	class MPIComm, class NumaComm>
grid::Container* grid::Grid::TypeSelector<Container, Level, MPIComm, NumaComm, magic::NullType>
	::createContainer(Grid &grid)
{
	assert(false);
	return 0L;
}

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

		unsigned int blockSizes[MAX_DIMENSIONS];
		for (unsigned int i = 0; i < MAX_DIMENSIONS; i++) {
			std::string sizeName = "BLOCK_SIZE_" + utils::StringUtils::toString(i);
			blockSizes[i] = param(sizeName.c_str(), 0, level); // 0 -> use default
		}

		return m_containers[m_numa.domainId()]->init(filename,
				param("VARIABLE", "z"),
				blockSizes,
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


//template<class Container, class Level, typename TypeList>
//grid::Container* grid::Grid::test(types::Type* type)

/**
 * Initializes all the containers
 */
void grid::Grid::initContainers()
{
	enum {
		FULL,
		CACHED,
		PASS_THROUGH,
		UNKNOWN
	} containerType = UNKNOWN;

	typedef magic::MakeTypelist<
			types::BasicType<unsigned char>,
			types::BasicType<int>,
			types::BasicType<long>,
			types::BasicType<float>,
			types::BasicType<double>,
			types::ArrayType<unsigned char>,
			types::ArrayType<int>,
			types::ArrayType<long>,
			types::ArrayType<float>,
			types::ArrayType<double>,
			types::StructType<unsigned char>,
			types::StructType<int>,
			types::StructType<long>,
			types::StructType<float>,
			types::StructType<double>
	>::result typelist;

	// Select the container type
	std::string gridType = param("GRID", "FULL");
	if (gridType == "CACHED")
		containerType = CACHED;
	else if (gridType == "PASS_THROUGH")
		containerType = PASS_THROUGH;
	else {
		containerType = FULL;
		if (gridType != "FULL") {
			logWarning(m_comm.rank()) << "ASAGI: Unknown grid type:" << gridType;
			logWarning(m_comm.rank()) << "ASAGI: Assuming FULL";
		}
	}

	// Initialize the container
	m_containers.resize(m_numa.totalDomains());
	for (std::vector<Container*>::iterator it = m_containers.begin();
			it != m_containers.end(); it++) {
		switch (containerType) {
		case FULL:
			*it = TypeSelector<SimpleContainer, level::FullDefault, magic::NullType, magic::NullType, typelist>::createContainer(*this);
			break;
		case PASS_THROUGH:
			*it = TypeSelector<SimpleContainer, level::PassThrough, magic::NullType, magic::NullType, typelist>::createContainer(*this);
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
