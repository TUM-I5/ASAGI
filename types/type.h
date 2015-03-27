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
 * @copyright 2012-2013 Sebastian Rettenberger <rettenbs@in.tum.de>
 */

#ifndef TYPES_TYPE_H
#define TYPES_TYPE_H

#include <asagi.h>

#include <cstring>

namespace io {
	class NetCdfReader;
}

/**
 * @brief Types specific code
 */
namespace types
{

/**
 * Describes the type of the variable stored in each grid cell.
 * 
 * This is a base class for arbitrary types with some default implemtentations.
 */
class Type
{
public:
	/**
	 * Empty destructor, makes sure constructor
	 * of child classes is called
	 */
	virtual ~Type() { }
	
	/**
	 * @return The size of the variable
	 */
	virtual unsigned int size() const = 0;
	
#ifndef ASAGI_NOMPI
	/**
	 * @return The corresponding MPI_Datatype for this type
	 */
	virtual MPI_Datatype getMPIType() = 0;
#endif // ASAGI_NOMPI
	
protected:
	/**
	 * Copies the data from <code>data</code> to <code>buf</code>
	 */
	static void copy(const void* data, void* buf, unsigned int size)
	{
		memcpy(buf, data, size);
	}
};

}

// Use this macro in subclasses to implement size(). It assumes you have
// size_static() defined. We use size() for dynamic linking and size_static()
// for static linking
#define TYPE_SIZE_FUNC unsigned int size() const { return size_static(); }

#endif // TYPES_TYPE_H
