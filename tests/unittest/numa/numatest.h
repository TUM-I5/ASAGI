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

#include <pthread.h>

#include "globaltest.h"

#include "numa/numa.h"

class NumaTest : public CxxTest::TestSuite
{
	numa::Numa* numa1;
	numa::Numa* numa2;

	unsigned int threadId2;
	bool master2;
	asagi::Grid::Error err2;

public:
	void setUp(void)
	{
		numa1 = new numa::Numa;
		numa2 = new numa::Numa;
		numa2->setThreads(2);
	}

	void tearDown(void)
	{
		delete numa1;
	}

	void testRegisterThread(void)
	{
		bool master;
		TS_ASSERT_EQUALS(numa1->registerThread(master),
				asagi::Grid::SUCCESS);
		TS_ASSERT(master);

		pthread_t thread;
		pthread_create(&thread, 0L, &NumaTest::thread2_registerThread, this);
		TS_ASSERT_EQUALS(numa2->registerThread(master),
				asagi::Grid::SUCCESS);

		pthread_join(thread, 0L);
		TS_ASSERT_EQUALS(err2, asagi::Grid::SUCCESS);

		if (numa2->totalDomains() == 1) {
			TS_ASSERT(master ^ master2);
		} else {
			TS_ASSERT(master);
			TS_ASSERT(master2);
		}

		TS_ASSERT_EQUALS(numa2->threadId(), 0u);
		TS_ASSERT_EQUALS(threadId2, 1u);
	}

	void testCreatecComm(void)
	{
		bool master;
		TS_ASSERT_EQUALS(numa1->registerThread(master),
				asagi::Grid::SUCCESS);
		TS_ASSERT(master);
		TS_ASSERT(numa1->createComm());

		pthread_t thread;
		pthread_create(&thread, 0L, &NumaTest::thread2_createComm, this);

		TS_ASSERT_EQUALS(numa2->registerThread(master),
				asagi::Grid::SUCCESS);

		numa::NumaComm* comm = numa2->createComm();
		TS_ASSERT(comm);

		pthread_join(thread, 0L);
		TS_ASSERT_EQUALS(err2, asagi::Grid::SUCCESS);
	}

private:
	static void* thread2_registerThread(void *ref)
	{
		NumaTest* ref2 = static_cast<NumaTest*>(ref);
		ref2->err2 = ref2->numa2->registerThread(ref2->master2);
		ref2->threadId2 = ref2->numa2->threadId();
		return 0L;
	}

	static void* thread2_createComm(void *ref)
	{
		NumaTest* ref2 = static_cast<NumaTest*>(ref);
		ref2->err2 = ref2->numa2->registerThread(ref2->master2);
		if (ref2->err2 != asagi::Grid::SUCCESS)
			return 0L;

		if (ref2->numa2->totalDomains() > 1)
			ref2->numa2->createComm();

		return 0L;
	}
};
