#include <fstream>
#include <iostream>
#include <netcdf>

#define NC_FILENAME "test.nc"
#define HEADER_FILENAME "tests.h"

#define WIDTH 512
#define HEIGHT 512

using namespace netCDF;

int main()
{
	// Create the nc file
	NcFile file(NC_FILENAME, NcFile::replace);
	
	NcDim x = file.addDim("x", WIDTH);
	NcDim y = file.addDim("y", HEIGHT);
	
	std::vector<NcDim> dims(2);
	dims[0] = y;
	dims[1] = x;
	
	NcFloat floatType;
	
	NcVar z = file.addVar("z", floatType, dims);
	
	float values[HEIGHT][WIDTH];
	
	for (unsigned int i = 0; i < HEIGHT; i++) {
		for (unsigned int j = 0; j < WIDTH; j++) {
			values[i][j] = i * WIDTH + j;
		}
	}
	
	z.putVar(values);
	
	// Create the header file
	std::ofstream headerFile;
	headerFile.open(HEADER_FILENAME);
	headerFile << "#ifndef TESTS_H\n";
	headerFile << "#define TESTS_H\n";
	
	headerFile << "#define NC_FILENAME \"" << NC_FILENAME << "\"\n";
	headerFile << "#define NC_WIDTH " << WIDTH << "\n";
	headerFile << "#define NC_HEIGHT " << HEIGHT << "\n";
	
	headerFile << "#endif // TESTS_H\n";
	headerFile.close();
	
	return 0;
}