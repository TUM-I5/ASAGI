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

#include "globaltest.h"
#include "testdefines.h"

#undef NDEBUG

#include "io/netcdfreader.h"

class NetCdfReaderTest : public CxxTest::TestSuite
{
	io::NetCdfReader* file3d;
public:
	void setUp(void)
	{
		file3d = new io::NetCdfReader("../../" NC_3D, 0);
		TS_ASSERT_EQUALS(file3d->open(), asagi::Grid::SUCCESS);
	}
	
	void tearDown(void)
	{
		delete file3d;
	}
	
	void testGetDimensions(void)
	{
		TS_ASSERT_EQUALS(file3d->dimensions(), 3u);
	}
	
	void testGetSize(void)
	{
		TS_ASSERT_EQUALS(file3d->getSize(0),
			static_cast<unsigned long>(WIDTH));
		TS_ASSERT_EQUALS(file3d->getSize(1),
			static_cast<unsigned long>(LENGTH));
		TS_ASSERT_EQUALS(file3d->getSize(2),
			static_cast<unsigned long>(HEIGHT));
	}
	
	/**
	 * @todo This test only tests 3d grids at the moment
	 */
	void testGetBlock(void)
	{
		float values[10][10][10];

		size_t offsets[3] = {0, 0, 0};
		size_t sizes[3] = {10, 10, 10};
		TS_ASSERT_THROWS_NOTHING(file3d->getBlock<float>(values, offsets, sizes));
		for (int i = 0; i < 10; i++)
			for (int j = 0; j < 10; j++)
				for (int k = 0; k < 10; k++)
					TS_ASSERT_EQUALS(values[i][j][k], calcValueAt(i, j, k, offsets));
		
		size_t offsets2[3] = {9, 4, 63};
		TS_ASSERT_THROWS_NOTHING(file3d->getBlock<float>(values, offsets2, sizes));
		for (int i = 0; i < HEIGHT-63; i++)
			for (int j = 0; j < 10; j++)
				for (int k = 0; k < 10; k++)
					TS_ASSERT_EQUALS(values[i][j][k], calcValueAt(i, j, k, offsets2));

		size_t offsets3[3] = {0, 62, 21};
		TS_ASSERT_THROWS_NOTHING(file3d->getBlock<float>(values, offsets3, sizes));
		for (int i = 0; i < 10; i++)
			for (int j = 0; j < LENGTH-62; j++)
				for (int k = 0; k < 10; k++)
					TS_ASSERT_EQUALS(values[i][j][k], calcValueAt(i, j, k, offsets3));

		size_t offsets4[3] = {65, 10, 0};
		TS_ASSERT_THROWS_NOTHING(file3d->getBlock<float>(values, offsets4, sizes));
		for (int i = 0; i < 10; i++)
			for (int j = 0; j < 10; j++)
				for (int k = 0; k < LENGTH-65; k++)
					TS_ASSERT_EQUALS(values[i][j][k], calcValueAt(i, j, k, offsets4));

		float values2[64][64][64];
		size_t offsets5[3] = {55, 55, 0};
		size_t sizes2[3] = {64, 64, 64};
		TS_ASSERT_THROWS_NOTHING(file3d->getBlock<float>(values2, offsets5, sizes2));
		for (int i = 0; i < 64; i++)
			for (int j = 0; j < LENGTH-55; j++)
				for (int k = 0; k < WIDTH-55; k++)
					TS_ASSERT_EQUALS(values2[i][j][k], calcValueAt(i, j, k, offsets5));

		// Test float to double conversion
		double values3[10][10][10];
		TS_ASSERT_THROWS_NOTHING(file3d->getBlock<double>(values3, offsets3, sizes));
		for (int i = 0; i < 10; i++)
			for (int j = 0; j < LENGTH-62; j++)
				for (int k = 0; k < 10; k++)
					TS_ASSERT_EQUALS(values3[i][j][k], calcValueAt(i, j, k, offsets3));

		// Test void
		/*TS_ASSERT_THROWS_NOTHING*/(file3d->getBlock<void>(values2, offsets5, sizes2));
		for (int i = 0; i < 64; i++)
			for (int j = 0; j < LENGTH-55; j++)
				for (int k = 0; k < WIDTH-55; k++)
					TS_ASSERT_EQUALS(values2[i][j][k], calcValueAt(i, j, k, offsets5));
	}

private:
	/**
	 * @return The value that should be in the netcdf file at x, y, z
	 */
	float calcValueAt(int z, int y, int x, size_t offsets[3])
	{
		return ((z+offsets[2]) * LENGTH + (y+offsets[1])) * WIDTH + (x+offsets[0]);
	}
};
