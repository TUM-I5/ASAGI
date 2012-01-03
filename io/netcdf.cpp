#include <math.h>

#include "netcdf.h"

io::NetCdf::NetCdf(const char* fileName)
	: file(fileName, NcFile::ReadOnly)
{
	NcVar* z;
	
	if (!file.is_valid()) {
		// Could not open file
		error = true;
		return;
	}
	
	z = file.get_var("z");
	if (z->type() != ncFloat) {
		// Only float is supported at the moment
		error = true;
		return;
	}
	
	dimSwitched = (z->get_dim(0) != file.get_dim("x"));
	
	error = false;
}

bool io::NetCdf::hasError()
{
	return error;
}

unsigned long io::NetCdf::getXDim()
{
	return file.get_dim("x")->size();
}

unsigned long io::NetCdf::getYDim()
{
	return file.get_dim("y")->size();
}

bool io::NetCdf::isDimSwitched()
{
	return dimSwitched;
}

float io::NetCdf::getXOffset()
{
	NcVar* x = file.get_var("x");
	
	if (x->is_valid())
		return x->as_float(0);
	
	return 0;
}

float io::NetCdf::getYOffset()
{
	NcVar* y = file.get_var("y");
	
	if (y->is_valid())
		return y->as_float(0);
	
	return 0;
}

float io::NetCdf::getXScaling()
{
	// TODO
	return 1;
}

float io::NetCdf::getYScaling()
{
	// TODO
	return 1;
}

float io::NetCdf::getMin()
{
	NcAtt* range = file.get_var("z")->get_att("actual_range");
	if (range->is_valid())
		return range->as_float(0);
	
	return NAN;
}

float io::NetCdf::getMax()
{
	NcAtt* range = file.get_var("z")->get_att("actual_range");
	if (range->is_valid())
		return range->as_float(1);
	
	return NAN;
}

float* io::NetCdf::getAll()
{
	long x, y;
	NcVar* z;
	
	if (dimSwitched) {
		y = getXDim();
		x = getYDim();
	} else {
		x = getXDim();
		y = getYDim();
	}
	
	float* result = new float[x * y];
	
	z = file.get_var("z");
	z->set_cur(0, 0);
	z->get(result, x, y);
	
	return result;
}

float io::NetCdf::getDefault()
{
	// TODO
	return 0;
}
