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

#ifndef TYPES_ARRAYTYPE_H
#define TYPES_ARRAYTYPE_H

#include "asagi.h"

#include <mutex>

#include "basictype.h"
#include "io/netcdfreader.h"
#include "mpi/scorephelper.h"
#include "threads/mutex.h"

namespace types {

/**
 * We use {@link types::BasicType}. This way {@link #convert\<B\>}  convert
 * the first element of the array to the correct data type. One
 * can use {@link #convert} to get the whole array.
 */
template<typename T> class ArrayType : public BasicType<T>
{
private:
	/** Number of elements in the array */
	unsigned int m_arraySize;
	
#ifndef ASAGI_NOMPI
	/** The MPI_Datatype representing this type*/
	MPI_Datatype m_mpiType;
#endif // ASAGI_NOMPI
	
	/** Lock for the check */
	threads::Mutex m_lock;

public:
	ArrayType()
		: m_arraySize(0)
#ifndef ASAGI_NOMPI
		, m_mpiType(MPI_DATATYPE_NULL)
#endif // ASAGI_NOMPI
	{
	}
	
	virtual ~ArrayType()
	{
#ifndef ASAGI_NOMPI
		MPI_FUN(MPI_Type_free)(&m_mpiType);
#endif // ASAGI_NOMPI
	}

	/**
	 * @copydoc BasicType::check(const io::NetCdfReader&)
	 */
	asagi::Grid::Error check(const io::NetCdfReader &file)
	{
		if (file.getVarSize() % sizeof(T) != 0)
			return asagi::Grid::INVALID_VAR_SIZE;
		
		unsigned int arraySize = file.getVarSize() / sizeof(T);

		// Lock the array size
		m_lock.lock();

		if (m_arraySize == 0) {
			m_arraySize = arraySize;

			m_lock.unlock();

#ifndef ASAGI_NOMPI
			// Create the mpi datatype
			if (MPI_FUN(MPI_Type_contiguous)(m_arraySize, BasicType<T>::getMPIType(),
					&m_mpiType) != MPI_SUCCESS)
				return asagi::Grid::MPI_ERROR;
			if (MPI_FUN(MPI_Type_commit)(&m_mpiType) != MPI_SUCCESS)
				return asagi::Grid::MPI_ERROR;
#endif // ASAGI_NOMPI
		} else {
			std::lock_guard<threads::Mutex> lock(m_lock, std::adopt_lock);

			if (arraySize != m_arraySize)
				return asagi::Grid::WRONG_SIZE;
		}
		
		return asagi::Grid::SUCCESS;
	}

	unsigned int size() const
	{
		return m_arraySize * sizeof(T);
	}

#ifndef ASAGI_NOMPI
	MPI_Datatype getMPIType() const
	{
		return m_mpiType;
	}
#endif // ASAGI_NOMPI

	/**
	 * @copydoc BasicType::load
	 */
	void load(io::NetCdfReader &file,
		const size_t *offset,
		const size_t *size,
		void *buf) const
	{
		file.getBlock<void>(buf, offset, size);
	}

	/**
	 * @copydoc BasicType::convert(const void*, void*)
	 */
	void convert(const void* data, void* buf) const
	{
		Type::copy(data, buf, m_arraySize * sizeof(T));
	}
};

}

#endif // TYPES_ARRAYTYPE_H
