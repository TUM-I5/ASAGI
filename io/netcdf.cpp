#include <math.h>
#include <netcdf>

#include "netcdf.h"

using namespace netCDF;
using namespace netCDF::exceptions;

io::NetCdf::NetCdf(const char* filename) :
	m_filename(filename)
{
	m_file = 0L;
}

io::NetCdf::~NetCdf()
{
	delete m_file;
}

bool io::NetCdf::open()
{
	try {
		m_file = new NcFile(m_filename, NcFile::read);
	} catch (NcException& e) {
		// Coudl not open file
		
		m_file = 0L;
		return false;
	}
	
	NcVar z = m_file->getVar("z");
		
	if (z.isNull())
		return false;
	
	if (z.getType().getTypeClass() != NcType::nc_FLOAT)
		// Only float is supported at the moment
		return false;
	
	m_dimSwitched = (z.getDim(0) != m_file->getDim("y"));
	
	return true;
}

bool io::NetCdf::isOpen() const
{
	return m_file != 0L;;
}

unsigned long io::NetCdf::getXDim()
{
	return m_file->getDim("x").getSize();
}

unsigned long io::NetCdf::getYDim()
{
	return m_file->getDim("y").getSize();
}

float io::NetCdf::getXOffset()
{
	float result;
	NcVar x = m_file->getVar("x");
	
	if (x.isNull())
		return 0;
	
	x.getVar(std::vector<size_t>(1, 0), &result);
	return result;
}

float io::NetCdf::getYOffset()
{
	float result;
	NcVar y = m_file->getVar("y");
	
	if (y.isNull())
		return 0;
	
	y.getVar(std::vector<size_t>(1, 0), &result);
	return result;
}

float io::NetCdf::getXScaling()
{
	float first, last;
	std::vector<size_t> index(1);
	NcVar x = m_file->getVar("x");
	unsigned long dim = getXDim();
	
	if (x.isNull())
		return 1;
	
	index[0] = 0;
	x.getVar(index, &first);
	index[0] = dim - 1;
	x.getVar(index, &last);
	
	return (last - first) / dim;
}

float io::NetCdf::getYScaling()
{
	float first, last;
	std::vector<size_t> index(1);
	NcVar y = m_file->getVar("y");
	unsigned long dim = getYDim();
	
	if (y.isNull())
		return 1;
	
	index[0] = 0;
	y.getVar(index, &first);
	index[0] = dim - 1;
	y.getVar(index, &last);
	
	return (last - first) / dim;
}

template<> void io::NetCdf::getVar<void>(void* var)
{
	NcVar z;
	unsigned int bytes = getVarSize();
	std::vector<size_t> start(2, 0);
	std::vector<size_t> count(2);
	std::vector<ptrdiff_t> stride(2, 1);
	std::vector<ptrdiff_t> imap(2);
	
	if (m_dimSwitched) {
		count[0] = getXDim();
		count[1] = getYDim();
		
		imap[0] = bytes;
		imap[1] = count[0] * bytes;
	} else {
		count[0] = getYDim();
		count[1] = getXDim();
		
		imap[0] = count[1] * bytes;
		imap[1] = bytes;
	}
	
	z = m_file->getVar("z");
	
	z.getVar(start, count, stride, imap, var);
}

unsigned int io::NetCdf::getVarSize()
{
	return m_file->getVar("z").getType().getSize();
}

// template<> void io::NetCdf::getDefault<void>(void* defaultValue)
// {
// 	NcVar z;
// 	NcVarAtt att;
// 	
// 	z = m_file->getVar("z");
// 	
// 	try {
// 		att = z.getAtt("missing_value");
// 	} catch (NcException& e) {
// 		// Attribute missing
// 		memset(defaultValue, 0, z.getType().getSize());
// 		return;
// 	}
// 	
// 	att.getValues(defaultValue);
// }
