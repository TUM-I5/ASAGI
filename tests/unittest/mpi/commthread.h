/**
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 * SPDX-FileCopyrightText: 2015 Sebastian Rettenberger <rettenbs@in.tum.de>
 */

#include "asagi.h"

#include <unistd.h>

#include "globaltest.h"

#include "mpi/commthread.h"

class CommThreadTest : public CxxTest::TestSuite, mpi::Receiver {
  private:
  mpi::message_t received;

  public:
  void setUp() { TS_ASSERT_EQUALS(mpi::CommThread::commThread.init(-1), asagi::Grid::SUCCESS); }

  void testInit(void) { mpi::CommThread::commThread.finialize(); }

  void testRegister(void) {
    int tag;
    TS_ASSERT_EQUALS(mpi::CommThread::commThread.registerReceiver(MPI_COMM_WORLD, *this, tag),
                     asagi::Grid::SUCCESS);
    mpi::CommThread::commThread.unregisterReceiver(tag);

    mpi::CommThread::commThread.finialize();
  }

  void testSendRecv(void) {
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

  void recv(int sender, mpi::message_t data) { received = data; }
};
