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

unsigned long io::NetCdf::getMin()
{
	return file.get_var("z")->get_att("actual_range")->as_float(0);
}

unsigned long io::NetCdf::getMax()
{
	return file.get_var("z")->get_att("actual_range")->as_float(1);
}

float* io::NetCdf::getAll()
{
	long x, y;
	
	if (dimSwitched) {
		y = getXDim();
		x = getYDim();
	} else {
		x = getXDim();
		y = getYDim();
	}
	
	float* result = new float[x * y];
	
	file.get_var("z")->get(result, x, y);
	
	return result;
}

