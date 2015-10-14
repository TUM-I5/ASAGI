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

#include "asagi.h"

#include <unistd.h>

#include "globaltest.h"

#include "mpi/commthread.h"

class CommThreadTest : public CxxTest::TestSuite, mpi::Receiver
{
private:
	mpi::message_t received;

public:
	void setUp()
	{
		TS_ASSERT_EQUALS(mpi::CommThread::commThread.init(-1),
				asagi::Grid::SUCCESS);
	}

	void testInit(void)
	{
		mpi::CommThread::commThread.finialize();
	}

	void testRegister(void)
	{
		int tag;
		TS_ASSERT_EQUALS(mpi::CommThread::commThread.registerReceiver(MPI_COMM_WORLD, *this, tag),
				asagi::Grid::SUCCESS);
		mpi::CommThread::commThread.unregisterReceiver(tag);

		mpi::CommThread::commThread.finialize();
	}

	void testSendRecv(void)
	{
		int tag;
		TS_ASSERT_EQUALS(mpi::CommThread::commThread.registerReceiver(MPI_COMM_WORLD, *this, tag),
				asagi::Grid::SUCCESS);

		received = -1;

		int rank;
		MPI_Comm_rank(MPI_COMM_WORLD, &rank);
		mpi::CommThread::commThread.send(tag, rank, 42);

		MPI_Barrier(MPI_COMM_WORLD);

		mpi::CommThread::commThread.unregisterReceiver(tag);

		mpi::CommThread::commThread.finialize();

		mpi::message_t result = 42;
		TS_ASSERT_EQUALS(received, result);
	}

	void recv(int sender, mpi::message_t data)
	{
		received = data;
	}
};
