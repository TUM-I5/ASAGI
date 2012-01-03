#include <cstdlib>
#include <fstream>
#include <iostream>

#include "tests.h"

#define HEADER_FILENAME "perftests.h"
#define COUNT_ACCESS 100000

int rand(int max)
{
	return static_cast<float>(rand()) * max / RAND_MAX; 
}

int main()
{
	srand(time(0));
	
	// Create the header file
	std::ofstream headerFile;
	headerFile.open(HEADER_FILENAME);
	headerFile << "#ifndef PERFTESTS_H\n";
	headerFile << "#define PERFTESTSPERF_H\n";
	
	headerFile << "#define NC_FILENAME \"" << "../"NC_FILENAME << "\"\n";
	
	headerFile << "#define COUNT_ACCESS " << COUNT_ACCESS << "\n";
	
	// Generate random indices
	int indices[COUNT_ACCESS];
	
	for (unsigned int i = 0; i < COUNT_ACCESS; i++) {
		indices[i] = rand(NC_WIDTH * NC_HEIGHT);
	}
	
	// Direct access
	headerFile << "long INDEX[] = {";
	for (unsigned int i = 0; i < COUNT_ACCESS; i++) {
		headerFile << indices[i] << ",";
	}
	headerFile << "};\n";
	
	// 2D access
	headerFile << "long INDEX2D[] = {";
	for (unsigned int i = 0; i < COUNT_ACCESS; i++) {
		headerFile << (indices[i] % NC_WIDTH) << ","; // x
		headerFile << (indices[i] / NC_WIDTH) << ","; // y
	}
	headerFile << "};\n";
	
	headerFile << "#endif // PERFTESTS_H\n";
	headerFile.close();
	
	return 0;
}