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
 * @copyright 2012 Sebastian Rettenberger <rettenbs@in.tum.de>
 */

#ifndef FORTRAN_POINTERARRAY_H
#define FORTRAN_POINTERARRAY_H

#include <cassert>
#ifdef THREADSAFETY
#include <mutex>
#endif // THREADSAFETY
#include <vector>

#define NULL_INDEX -1

/**
 * @brief C++ code required to support the Fortran API
 */
namespace fortran
{

/**
 * Maps indices to pointers. The reverse mapping (from pointers to indices) is
 * done by the class itself.
 */
template<class T> class PointerArray
{
private:
	/** Array that maps to the correct pointer */
	std::vector<T*> m_pointers;
#ifdef THREADSAFETY
	/**
	 * Lock, we use to make sure only one thread at a time adds a pointer
	 */
	std::mutex m_lock;
#endif // THREADSAFETY
public:
	/**
	 * Add a new pointer to the map
	 * 
	 * @return The index which can be used to access the pointer
	 */
	int add(T* const p)
	{
#ifdef THREADSAFETY
		// Lock vector, otherwise the id (return value)
		// gets messed up
		std::lock_guard<std::mutex> lock(m_lock);
#endif // THREADSAFETY
		
		m_pointers.push_back(p);
		return m_pointers.size() - 1;
	}
	
	/**
	 * @return The pointer to the object for index <code>i</code>
	 */
	T* get(int i) const
	{
		assert(i >= 0 &&
			static_cast<unsigned int>(i) < m_pointers.size());
		
		return m_pointers[i];
	}
	
	/**
	 * Removes the pointer with index <code>i</code> from the map
	 */
	void remove(int i)
	{
		assert(i >= 0 &&
			static_cast<unsigned int>(i) < m_pointers.size());
		
		m_pointers[i] = 0L;
	}
};

}

#endif // FORTRAN_POINTERARRAY_H