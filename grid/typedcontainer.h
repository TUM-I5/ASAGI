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

#ifndef GRID_TYPEDCONTAINER_H
#define GRID_TYPEDCONTAINER_H

#include "container.h"

namespace grid
{

/**
 * A container with a specific element type
 */
template<class Type>
class TypedContainer : public Container
{
private:
	/**
	 * The type of values we save in the grid.
	 * This class implements all type specific operations.
	 */
	Type &m_type;

public:
	/**
	 * @copydoc Container::Container
	 */
	TypedContainer(mpi::MPIComm &comm,
			const numa::Numa &numa,
			Type &type,
			int timeDimension,
			ValuePosition valuePos)
		: Container(comm, numa, timeDimension, valuePos),
		  m_type(type)
	{
	}

	virtual ~TypedContainer() {}

	/**
	 * @return The type for this container
	 */
	const Type& type() const
	{
		return m_type;
	}

	/**
	 * @copydoc const Type& type() const
	 */
	Type& type()
	{
		return m_type;
	}
};

}

#endif // GRID_TYPEDCONTAINER_H
