#!/usr/bin/python

import math
from pnoise import pnoise

### Try to import netcdf ##

# Do some magic here, because I don't want to mess up my python installation
import os, sys
netcdf_folder = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'netcdf')
if netcdf_folder not in sys.path:
	sys.path.insert(0, netcdf_folder)
	
create_nc = False
try:
	from netCDF3 import Dataset
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
width = 128			#width of the resulting grid [1, inf)
height = 257		#height of the resulting grid [1, inf)
roughness = 0.2		#roughness of the island boundaries, 0.0 = totally smooth, 1.0 = white noise [0.0, 1.0]
x_scaling = 5.0		#x coordinate scaling, higher values result in more islands, smaller values in less (0.0, +inf)
y_scaling = 5.0		#y coordinate scaling, higher values result in more islands, smaller values in less (0.0, +inf)
out_scaling = 0.5	#output value scaling
out_offset = 0.5	#output value offset

def noise(x, y, z, depth, roughness):
	f = 1.0
	n = 0.0
	sum_f = 0.0
	
	for i in range(0, depth):
		n += f * pnoise(x, y, z)
		sum_f += f
		f *= roughness
		x *= 2.0
		y *= 2.0
		z *= 2.0
	
	return n / sum_f

	
depth = int(math.log(max(width, height), 2))

fileID = open("./out.vtk", 'w')
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
	nc = Dataset('out.nc', 'w')
	nc.createDimension('x', width)
	nc.createDimension('y', height)
	nc_values = nc.createVariable('z', 'f4', ('x', 'y'))

for j in range(0, height):
	for i in range(0, width):
		x = x_scaling * float(i) / float(width)
		y = y_scaling * float(j) / float(height)
		out = out_offset + out_scaling * noise(x, y, 0, depth, roughness)
		fileID.write("%f\n" %out)
		
		if create_nc:
			nc_values[i,j] = out

fileID.close()

if create_nc:
	nc.close()