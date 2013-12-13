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

#ifndef MPI_MUTEX_H
#define MPI_MUTEX_H

#include <asagi.h>

/**
 * @brief Extends the functionallity of MPI
 */
namespace mpi {

/**
 * See "Implementing Byte-Range Locks Using MPI One-Sided Communication" for
 * details about this algorithm.
 * 
 * This class only works with at least 2 MPI processes.
 * 
 * @warning Aquire and release are notthreadsafe at the moment. We do not need
 * this since the {@link blocks::BlockManager} is not threadsafe either.
 */
class Mutex
{
private:
	/** The communicator we use to create the window and for send/recv */
	MPI_Comm m_comm;
	
	/** Our rank in the communicator */
	int m_rank;
	/** Size of the communicator */
	int m_size;
	
	/** Rank were the lock array {@link m_lock} is stored */
	int m_homeRank;
	
	/**
	 * The lock array contains all the locks, only available
	 * on rank {@link m_homeRank} */
	long* m_lock;
	
	/** The window is used to store the locks */
	MPI_Win m_window;
	
	/**
	 * Allows to access all elements from {@link m_lock} without the
	 * current rank
	 */
	MPI_Datatype m_otherRanksType;
	
	/** The tag we use for send/recv */
	const int m_tag;
public:
	Mutex(int tag);
	virtual ~Mutex();
	
	asagi::Grid::Error init(MPI_Comm comm);
	
	void acquire(unsigned long block);
	
	void release(unsigned long block);
};

}

#endif // MPI_MUTEX_H
