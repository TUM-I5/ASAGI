#!/usr/bin/python

import sys

import numpy
import netCDF4
from netCDF4 import Dataset

TIME_STEPS = 600

oldfile = sys.argv[1]
newfile = sys.argv[2]

ncOld = Dataset(oldfile, 'r')
ncNew = Dataset(newfile, 'w', format=ncOld.file_format)

# Copy attributes
def copyAttributes(old, new):
	for name in old.ncattrs():
		if name == '_FillValue':
			continue
		new.__setattr__(name, old.__getattr__(name))

copyAttributes(ncOld, ncNew)

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
		copyAttributes(oldVariable, newVariable)
		newVariable[:] = oldVariable[:]
newVariable = ncNew.createVariable('time', 'f4', ('time',))
newVariable.long_name = 'time'
newVariable.actual_range = [0., TIME_STEPS]
newVariable.units = 'seconds since 0000-1-1 0:0:0'
newVariable[:] = numpy.arange(0, 300, 300/float(TIME_STEPS))

# Create variables
for name in ncOld.variables:
	if not name in ncOld.dimensions:
		# A real variable
		oldVariable = ncOld.variables[name]
		newVariable = ncNew.createVariable(name, oldVariable.dtype, ('time',) + oldVariable.dimensions)
		copyAttributes(oldVariable, newVariable)
		oldValues = oldVariable[:]
		for i in range(TIME_STEPS):
			print "Iteration: "+str(i)
			newVariable[i] = [i*v/float(TIME_STEPS) for v in oldValues]

ncOld.close()
ncNew.close()
