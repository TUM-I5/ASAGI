/**
 * @file
 * 
 * @author Sebastian Rettenberger <rettensb@in.tum.de>
 */

#include "globaltest.h"
#include "tests.h"

#include "types/arraytype.h"

class ArrayTypeTest : public CxxTest::TestSuite
{
	io::NetCdf* file;
public:
	void setUp(void)
	{
		MPIHelper::setUp();
		
		file = new io::NetCdf("../../"NC_1D, 0);
		file->open();
	}
	
	void tearDown(void)
	{
		delete file;
		
		MPIHelper::tearDown();
	}
	
	void testGetSize(void)
	{
		types::ArrayType<float> type;
		type.check(*file);
		
		TS_ASSERT_EQUALS(type.getSize(), sizeof(float));
	}
};