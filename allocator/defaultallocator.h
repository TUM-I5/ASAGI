/**
 * @file
 *  This file is part of ASAGI
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
 * @copyright 2013 Sebastian Rettenberger <rettenbs@in.tum.de>
 */

#ifndef ALLOCATOR_DEFAULTALLOCATOR_H
#define ALLOCATOR_DEFAULTALLOCATOR_H

#include <asagi.h>

#include "allocator/allocator.h"

namespace allocator
{

/**
 * This allocator uses default C++ new/delete mechanism
 */
template<typename T>
class DefaultAllocator : public Allocator<T>
{
public:
	/**
	 * Empty constructor, required by newer gcc versions
	 */
	DefaultAllocator()
	{
	}

	asagi::Grid::Error allocate(size_t size, T* &ptr) const
	{
		ptr = new T[size];
		return asagi::Grid::SUCCESS;
	}

	void free(T *ptr) const
	{
		delete [] ptr;
	}

public:
	static const DefaultAllocator<T> allocator;
};

/**
 * Provides a default instance for each type
 */
template<typename T>
const DefaultAllocator<T> DefaultAllocator<T>::allocator;

}

#endif /* ALLOCATOR_DEFAULTALLOCATOR_H */
