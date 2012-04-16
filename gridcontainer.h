#ifndef GRIDCONTAINER_H
#define GRIDCONTAINER_H

#include <asagi.h>

#include "fortran/pointerarray.h"
#include "types/type.h"

class Grid;

class GridContainer : public asagi::Grid
{
public:
	enum ValuePos { CELL_CENTERED, VERTEX_CENTERED };
private:
	/** Id of the grid, used for the fortran <-> c interface */
	int m_id;
	
	/** Number of levels this grid container has */
	unsigned int m_levels;
	
	/** All grids we control */
	::Grid **m_grids;
	
#ifndef ASAGI_NOMPI
	/** The communicator we use */
	MPI_Comm m_communicator;
#endif // ASAGI_NOMPI
	/** Rank of this process */
	int m_mpiRank;
	/** Size of the MPI communicator */
	int m_mpiSize;
	
	/**
	 * The type of values we save in the grid.
	 * This class implements all type specific operations.
	 */
	types::Type *m_type;
	
	double m_minX;
	double m_minY;
	double m_minZ;
	double m_maxX;
	double m_maxY;
	double m_maxZ;
	
	ValuePos m_valuePos;
public:
	GridContainer(Type type, bool isArray = false,
		unsigned int hint = asagi::NO_HINT,
		unsigned int level = 1);
	virtual ~GridContainer();
	
#ifndef ASAGI_NOMPI
	Error setComm(MPI_Comm comm = MPI_COMM_WORLD);
#endif // ASAGI_NOMPI
	Error setParam(const char* name, const char* value,
		unsigned int level = 0);
	Error open(const char* filename, unsigned int level = 0);
	
	double getXMin()
	{
		return m_minX;
	}
	double getYMin()
	{
		return m_minY;
	}
	double getZMin()
	{
		return m_minZ;
	}
	double getXMax()
	{
		return m_maxX;
	}
	double getYMax()
	{
		return m_maxY;
	}
	double getZMax()
	{
		return m_maxZ;
	}
	
	unsigned int getVarSize()
	{
		return m_type->getSize();
	}
	
	char getByte1D(double x, unsigned int level = 0)
	{
		return getByte3D(x, 0, 0, level);
	}
	int getInt1D(double x, unsigned int level = 0)
	{
		return getInt3D(x, 0, 0, level);
	}
	long getLong1D(double x, unsigned int level = 0)
	{
		return getLong3D(x, 0, 0, level);
	}
	float getFloat1D(double x, unsigned int level = 0)
	{
		return getFloat3D(x, 0, 0, level);
	}
	double getDouble1D(double x, unsigned int level = 0)
	{
		return getDouble3D(x, 0, 0, level);
	}
	void getBuf1D(void* buf, double x, unsigned int level = 0)
	{
		getBuf3D(buf, x, 0, 0, level);
	}
	
	char getByte2D(double x, double y, unsigned int level = 0)
	{
		return getByte3D(x, y, 0, level);
	}
	int getInt2D(double x, double y, unsigned int level = 0)
	{
		return getInt3D(x, y, 0, level);
	}
	long getLong2D(double x, double y, unsigned int level = 0)
	{
		return getLong3D(x, y, 0, level);
	}
	float getFloat2D(double x, double y, unsigned int level = 0)
	{
		return getFloat3D(x, y, 0, level);
	}
	double getDouble2D(double x, double y, unsigned int level = 0)
	{
		return getDouble3D(x, y, 0, level);
	}
	void getBuf2D(void* buf, double x, double y, unsigned int level = 0)
	{
		getBuf3D(buf, x, y, 0, level);
	}
	
	char getByte3D(double x, double y = 0, double z = 0,
		unsigned int level = 0);
	int getInt3D(double x, double y = 0, double z = 0,
		unsigned int level = 0);
	long getLong3D(double x, double y = 0, double z = 0,
		unsigned int level = 0);
	float getFloat3D(double x, double y = 0, double z = 0,
		unsigned int level = 0);
	double getDouble3D(double x, double y = 0, double z = 0,
		unsigned int level = 0);
	void getBuf3D(void* buf, double x, double y = 0, double z = 0,
		unsigned int level = 0);
	
	bool exportPng(const char* filename, unsigned int level = 0);
	
#ifndef ASAGI_NOMPI
	MPI_Comm getMPICommunicator()
	{
		return m_communicator;
	}
#endif // ASAGI_NOMPI
	
	int getMPIRank()
	{
		return m_mpiRank;
	}
	
	int getMPISize()
	{
		return m_mpiSize;
	}
	
	types::Type& getType()
	{
		return *m_type;
	}
	
	ValuePos getValuePos()
	{
		return m_valuePos;
	}
	
	/**
	 * Converts the C pointer of the grid to the Fortran identifier
	 * 
	 * @return The unique index of the grid container
	 */
	int c2f()
	{
		return m_id;
	}
private:
	static fortran::PointerArray<GridContainer> m_pointers;
public:
	static GridContainer* f2c(int i)
	{
		return m_pointers.get(i);
	}
};

#endif // GRIDCONTAINER_H
