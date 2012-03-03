#include <math.h>
#include <limits>

#include "netcdf.h"

#include "debug/dbg.h"

#define DIM_NOT_MAPPED "<not mapped>"

using namespace asagi;

using namespace netCDF;
using namespace netCDF::exceptions;

io::NetCdf::NetCdf(const char* filename, int rank) :
	m_filename(filename), m_rank(rank)
{
	m_file = 0L;
}

io::NetCdf::~NetCdf()
{
	delete m_file;
}

Grid::Error io::NetCdf::open(const char* varname)
{
	try {
		m_file = new NcFile(m_filename, NcFile::read);
	} catch (NcException& e) {
		// Could not open file
		
		m_file = 0L;
		return Grid::NOT_OPEN;
	}
	
	m_variable = m_file->getVar(varname);
		
	if (m_variable.isNull())
		return Grid::VAR_NOT_FOUND;
	
	m_dimensions = m_variable.getDimCount();
	switch (m_dimensions) {
	case 1:
		m_nameX = m_variable.getDim(0).getName();
		m_nameY = DIM_NOT_MAPPED;
		m_nameZ = DIM_NOT_MAPPED;
		break;
	case 2:
		m_nameX = m_variable.getDim(1).getName();
		m_nameY = m_variable.getDim(0).getName();
		m_nameZ = DIM_NOT_MAPPED;
		break;
	case 3:
		m_nameX = m_variable.getDim(2).getName();
		m_nameY = m_variable.getDim(1).getName();
		m_nameZ = m_variable.getDim(0).getName();
		break;
	default:
		dbgDebug(m_rank) << "Unsupported number of variable dimensions:"
			<< m_dimensions;
		return Grid::UNSUPPORTED_DIMENSIONS;
	}
	
	dbgDebug(m_rank) << "Dimension mapping: x :=" << m_nameX << "y :="
		<< m_nameY << "z :=" << m_nameZ;
	
	return Grid::SUCCESS;
}

bool io::NetCdf::isOpen() const
{
	return m_file != 0L;;
}

unsigned long io::NetCdf::getXDim()
{
	return m_file->getDim(m_nameX).getSize();
}

unsigned long io::NetCdf::getYDim()
{
	if (m_dimensions < 2)
		return 1;
	return m_file->getDim(m_nameY).getSize();
}

unsigned long io::NetCdf::getZDim()
{
	if (m_dimensions < 3)
		return 1;
	return m_file->getDim(m_nameZ).getSize();
}

double io::NetCdf::getXOffset()
{
	double result;
	NcVar x = m_file->getVar(m_nameX);
	
	if (x.isNull())
		return 0;
	
	x.getVar(std::vector<size_t>(1, 0), &result);
	return result;
}

double io::NetCdf::getYOffset()
{
	double result;
	NcVar y = m_file->getVar(m_nameY);
	
	if (y.isNull())
		return 0;
	
	y.getVar(std::vector<size_t>(1, 0), &result);
	return result;
}

double io::NetCdf::getZOffset()
{
	double result;
	NcVar z = m_file->getVar(m_nameZ);
	
	if (z.isNull())
		return 0;
	
	z.getVar(std::vector<size_t>(1, 0), &result);
	return result;
}

double io::NetCdf::getXScaling()
{
	double first, last;
	std::vector<size_t> index(1);
	NcVar x;
	unsigned long dim;
	
	dim = getXDim();
	if (dim == 1)
		return std::numeric_limits<double>::infinity();
	
	x = m_file->getVar(m_nameX);
	if (x.isNull())
		return 1;
	
	index[0] = 0;
	x.getVar(index, &first);
	index[0] = dim - 1;
	x.getVar(index, &last);
	
	return (last - first) / (dim - 1);
}

double io::NetCdf::getYScaling()
{
	double first, last;
	std::vector<size_t> index(1);
	NcVar y;
	unsigned long dim;
	
	if (m_dimensions < 2)
		return 0.;
	
	dim = getYDim();
	
	if (dim == 1)
		return std::numeric_limits<double>::infinity();
	
	y = m_file->getVar(m_nameY);
	if (y.isNull())
		return 1;
	
	index[0] = 0;
	y.getVar(index, &first);
	index[0] = dim - 1;
	y.getVar(index, &last);
	
	return (last - first) / (dim - 1);
}

double io::NetCdf::getZScaling()
{
	double first, last;
	std::vector<size_t> index(1);
	NcVar z;
	unsigned long dim;
	
	if (m_dimensions < 3)
		return 0.;
	
	dim = getZDim();
	if (dim == 1)
		return std::numeric_limits<double>::infinity();
	
	z = m_file->getVar(m_nameZ);
	if (z.isNull())
		return 1;
	
	index[0] = 0;
	z.getVar(index, &first);
	index[0] = dim - 1;
	z.getVar(index, &last);
	
	return (last - first) / (dim - 1);
}