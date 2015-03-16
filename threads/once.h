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

#ifndef THREADS_ONCE_H
#define THREADS_ONCE_H

#include <mutex>

#include "mutex.h"

namespace threads
{

/**
 * A small class that executes a function exactly once
 */
class Once {
private:
	/** Already executed */
	bool m_state;

	/** Mutex to lock the state */
	Mutex m_mutex;

public:
	Once()
		: m_state(false)
	{ }

	/**
	 * Executes <code>func</code> of <code>obj</code>
	 * exactly once. When this function returns, <code>func</code>
	 * has been executed.
	 */
	template<class T>
	void saveExec(T &obj, void (T::*func)())
	{
		// Default case
		if (m_state)
			return;

		// Lock the state and double check
		std::lock_guard<Mutex> lock(m_mutex);
		if (m_state)
			return;

		// Save execute:
		// Execute the function before freeing the lock
		(obj.*func)();

		m_state = true;
	}
};

}

#endif // THREADS_ONCE_H
