#!/usr/bin/python

import sys

import numpy
import netCDF4
from netCDF4 import Dataset

TIME_STEPS = 600

oldfile = sys.argv[1]
newfile = sys.argv[2]

ncOld = Dataset(oldfile, 'r')
ncNew = Dataset(newfile, 'w')

# Create dimensions
for name in ncOld.dimensions:
	ncNew.createDimension(name, len(ncOld.dimensions[name]))
ncNew.createDimension('time', TIME_STEPS)

# Create dimension variables
for name in ncOld.variables:
	if name in ncOld.dimensions:
		# A dimension variable
		oldVariable = ncOld.variables[name]
		newVariable = ncNew.createVariable(name, oldVariable.dtype, (name,))
		newVariable[:] = oldVariable[:]
newVariable = ncNew.createVariable('time', 'f4', ('time',))
newVariable[:] = numpy.arange(0, 300, 300/float(TIME_STEPS))

# Create variables
for name in ncOld.variables:
	if not name in ncOld.dimensions:
		# A real variable
		oldVariable = ncOld.variables[name]
		newVariable = ncNew.createVariable(name, oldVariable.dtype, ('time',) + oldVariable.dimensions)
		oldValues = oldVariable[:]
		for i in range(TIME_STEPS):
			print "Iteration: "+str(i)
			newVariable[i] = [i*v/float(TIME_STEPS) for v in oldValues]

ncOld.close()
ncNew.close()
