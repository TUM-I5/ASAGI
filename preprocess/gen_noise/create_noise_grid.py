#!/usr/bin/python

import math
from pnoise import noise2D, noise3D
import numpy

### Try to import netcdf ##
	
create_nc = False
try:
	import netCDF4
	from netCDF4 import Dataset
	create_nc = True
except ImportError:
	# Install numpy-devel files
	## Download http://code.google.com/p/netcdf4-python/downloads/detail?name=netCDF4-0.9.3.tar.gz
	# Extract
	# cd netCDF4-0.9.3
	# python setup-nc3.py build
	# sudo python setup-nc3.py install
	
	import warnings
	warnings.warn("NetCDF not found, out.nc is not created")


#parameters
width = 513				#width of the resulting grid [1, inf)
height = 513			#height of the resulting grid [1, inf)
roughness = 0.2			#roughness of the island boundaries, 0.0 = totally smooth, 1.0 = white noise [0.0, 1.0]
	
depth = int(math.log(height, 2))

fileID = open("./perm_random.vtk", 'w')
fileID.write('# vtk DataFile Version 2.0\n')
fileID.write('Perlin noise boundary\n')
fileID.write('ASCII\n')
fileID.write('DATASET STRUCTURED_POINTS\n')
fileID.write('DIMENSIONS %i %i %i\n' %(width, height, 1))
fileID.write('ORIGIN %g %g %g\n' %(-0.5, -0.5, 0.0))
fileID.write('SPACING %g %g %g\n' %(1.0, 1.0, 1.0))

fileID.write('\n')
fileID.write('POINT_DATA %i\n' %(width * height))

fileID.write('\n')
fileID.write('SCALARS coeff float 1\n')
fileID.write('LOOKUP_TABLE default\n')

if create_nc:
	nc = Dataset('perm_random.nc', 'w')
	nc.createDimension('x', width)
	nc.createDimension('y', height)
	nc_x = nc.createVariable('x', 'f4', ('x'))
	nc_y = nc.createVariable('y', 'f4', ('y'))
	nc_values = nc.createVariable('z', 'f4', ('x', 'y'))
else:
	nc_x = []
	nc_y = []

nc_x[:] = numpy.linspace(0.0, float(width)/float(height), width)
nc_y[:] = numpy.linspace(0.0, 1.0, height)

for j in range(0, height):
	y = nc_y[j]
	
	for i in range(0, width):
		x = nc_x[i]
		
		out = -7.0e-8 * (noise2D(10.0 * x - 2.0, 10.0 * y, depth, roughness) + 0.7 - 4.0 * y * (1.0 - y)) + 0.5e-8
		out = max(0.0, min(1.0e-8, out))

		#out = out_offset + out_scaling * noise(x, y, depth, roughness)
		fileID.write("%f\n" %out)
		
		if create_nc:
			nc_values[i,j] = out

fileID.close()

if create_nc:
	nc.close()
