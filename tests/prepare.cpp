/**
 * @file
 *  This file is part of ASAGI.
 * 
 *  ASAGI is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  ASAGI is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with ASAGI.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Diese Datei ist Teil von ASAGI.
 *
 *  ASAGI ist Freie Software: Sie koennen es unter den Bedingungen
 *  der GNU General Public License, wie von der Free Software Foundation,
 *  Version 3 der Lizenz oder (nach Ihrer Option) jeder spaeteren
 *  veroeffentlichten Version, weiterverbreiten und/oder modifizieren.
 *
 *  ASAGI wird in der Hoffnung, dass es nuetzlich sein wird, aber
 *  OHNE JEDE GEWAEHELEISTUNG, bereitgestellt; sogar ohne die implizite
 *  Gewaehrleistung der MARKTFAEHIGKEIT oder EIGNUNG FUER EINEN BESTIMMTEN
 *  ZWECK. Siehe die GNU General Public License fuer weitere Details.
 *
 *  Sie sollten eine Kopie der GNU General Public License zusammen mit diesem
 *  Programm erhalten haben. Wenn nicht, siehe <http://www.gnu.org/licenses/>.
 * 
 * @copyright 2012 Sebastian Rettenberger <rettenbs@in.tum.de>
 */

#include <fstream>
#include <iostream>
#include <netcdf>

#define NC_1D "1dgrid.nc"
#define NC_1DPSEUDO "1dpseudogrid.nc"
#define NC_2D "2dgrid.nc"
#define NC_3D "3dgrid.nc"
#define NC_PERFTEST NC_2D
#define HEADER_FILENAME "tests.h"

#define WIDTH 51
#define LENGTH 51
#define HEIGHT 51

using namespace netCDF;

int main()
{
	NcFloat floatType;
	NcVar var;
	NcDim x, y, z;
	
	// Create the nc files
	
	// 1d
	NcFile nc_1d(NC_1D, NcFile::replace);
	
	x = nc_1d.addDim("x", WIDTH);
	
	std::vector<NcDim> dims_1d(1);
	dims_1d[0] = x;
	
	var = nc_1d.addVar("z", floatType, dims_1d);
	
	float values_1d[WIDTH];
	
	for (unsigned int i = 0; i < WIDTH; i++)
		values_1d[i] = i;
	
	var.putVar(values_1d);
	
	// pseudo 1d
	NcFile nc_1dpseudo(NC_1DPSEUDO, NcFile::replace);
	
	x = nc_1dpseudo.addDim("x", WIDTH);
	y = nc_1dpseudo.addDim("y", 1);
	
	std::vector<NcDim> dims_2d(2);
	dims_2d[0] = y;
	dims_2d[1] = x;
	
	var = nc_1dpseudo.addVar("z", floatType, dims_2d);
	
	var.putVar(values_1d); // reuse values
	
	// 2d
	NcFile nc_2d(NC_2D, NcFile::replace);
	
	x = nc_2d.addDim("x", WIDTH);
	y = nc_2d.addDim("y", LENGTH);
	
	dims_2d[0] = y;
	dims_2d[1] = x;
	
	var = nc_2d.addVar("z", floatType, dims_2d);
	
	float values_2d[LENGTH][WIDTH];
	
	for (unsigned int i = 0; i < LENGTH; i++)
		for (unsigned int j = 0; j < WIDTH; j++)
			values_2d[i][j] = i * WIDTH + j;
	
	var.putVar(values_2d);
	
	// 3d
	NcFile file(NC_3D, NcFile::replace);
	
	x = file.addDim("x", WIDTH);
	y = file.addDim("y", LENGTH);
	z = file.addDim("t", HEIGHT);	// can't set this to z ...
	
	std::vector<NcDim> dims_3d(3);
	dims_3d[0] = z;
	dims_3d[1] = y;
	dims_3d[2] = x;
	
	var = file.addVar("z", floatType, dims_3d);
	
	std::vector<size_t> offset_3d(3, 0);
	std::vector<size_t> size_3d(3);
	
	size_3d[0] = 1;
	size_3d[1] = LENGTH;
	size_3d[2] = WIDTH;
	
	for (unsigned int i = 0; i < HEIGHT; i++) {
		offset_3d[0] = i;
		
		for (unsigned int j = 0; j < LENGTH; j++)
			for (unsigned int k = 0; k < WIDTH; k++)
				values_2d[j][k] = (i * LENGTH + j) * WIDTH + k;
		
		var.putVar(offset_3d, size_3d, values_2d);
	}
	
	// Create the header file
	std::ofstream headerFile;
	headerFile.open(HEADER_FILENAME);
	headerFile << "#ifndef TESTS_H\n";
	headerFile << "#define TESTS_H\n";
	
	headerFile << "#define NC_1D \"" << NC_1D << "\"\n";
	headerFile << "#define NC_1DPSEUDO \"" << NC_1DPSEUDO << "\"\n";
	headerFile << "#define NC_2D \"" << NC_2D << "\"\n";
	headerFile << "#define NC_3D \"" << NC_3D << "\"\n";
	headerFile << "#define NC_PERFTEST \"" << NC_PERFTEST << "\"\n";
	headerFile << "#define NC_WIDTH " << WIDTH << "\n";
	headerFile << "#define NC_LENGTH " << LENGTH << "\n";
	headerFile << "#define NC_HEIGHT " << HEIGHT << "\n";
	
	headerFile << "#endif // TESTS_H\n";
	headerFile.close();
	
	return 0;
}