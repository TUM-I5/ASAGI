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

#include "globaltest.h"

#include "threads/sync.h"

class SyncTest : public CxxTest::TestSuite
{
	threads::Sync sync;

	int value;
	int* pointer;
	bool ret2;

public:
	void testBarrier(void)
	{
		pthread_t thread;
		pthread_create(&thread, 0L, &SyncTest::thread2_barrier, this);

		TS_ASSERT(sync.barrier(2));
		TS_ASSERT_EQUALS(value, 42);

		TS_ASSERT(sync.barrier(2));

		pthread_join(thread, 0L);
		TS_ASSERT(ret2);

		TS_ASSERT_EQUALS(value, 5);
	}

	void testBroadcast(void)
	{
		pthread_t thread;
		pthread_create(&thread, 0L, &SyncTest::thread2_broadcast, this);

		pointer = 0L;
		int myvalue = 77;
		int* localp = &myvalue;
		TS_ASSERT(sync.broadcast(localp, 2, 0));

		pthread_join(thread, 0L);
		TS_ASSERT(ret2);

		TS_ASSERT(pointer);
		TS_ASSERT_EQUALS(*pointer, 77);
	}

private:
	static void* thread2_barrier(void *ref)
	{
		SyncTest* ref2 = static_cast<SyncTest*>(ref);

		ref2->value = 42;
		ref2->ret2 = ref2->sync.barrier(2);
		if (!ref2->ret2)
			return 0L;

		ref2->ret2 = ref2->sync.barrier(2);
		if (!ref2->ret2)
			return 0L;

		ref2->value = 5;

		return 0L;
	}

	static void* thread2_broadcast(void *ref)
	{
		SyncTest* ref2 = static_cast<SyncTest*>(ref);

		ref2->ret2 = ref2->sync.broadcast(ref2->pointer, 2, 1);
		if (!ref2->ret2)
			return 0L;

		return 0L;
	}
};
