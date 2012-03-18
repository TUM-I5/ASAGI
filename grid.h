#ifndef GRID_H
#define GRID_H

#include "gridcontainer.h"

#include "types/type.h"
#include "debug/dbg.h"

class GridContainer;

namespace io
{
	class NetCdf;
}

class Grid
{
private:
	/** The container we belong too */
	GridContainer &m_container;
	
	/** Name of the variable in the netcdf file (default: "z") */
	std::string m_variableName;
	
	io::NetCdf *m_inputFile;
	
	/** Total number of elements in x, y and z dimension */
	unsigned long m_dim[3];
	
	double offsetX;
	double offsetY;
	double offsetZ;
	
	/**
	 * used to calculate {@link getXMin()} and {@link getXMax()}
	 */
	double scalingX;
	double scalingY;
	double scalingZ;
	
	/**
	 * 1/{@link scalingX} in most cases (exceptions: {@link scalingX} = 0
	 * and {@link scalingX = inf}), used to convert coordinates to indices
	 */
	double scalingInvX;
	double scalingInvY;
	double scalingInvZ;
	
	/** Number of blocks in x, y and z dimension */
	unsigned long blocks[3];
	
	/** Number of values in x, y and z dimension in one block */
	unsigned long m_blockSize[3];
	
	/**
	 * 0, 1 or 2 if x, y or z is a time dimension (z is default if
	 * the HAS_TIME hint is specified);
	 * -2 if we don't have any time dimension;
	 * -1 if the time dimension is not (yet) specified
	 * <br>
	 * Declare as signed, to remove compiler warning
	 */
	signed char m_timeDimension;
public:
	Grid(GridContainer &container, unsigned int hint = asagi::NO_HINT);
	virtual ~Grid();
	
	asagi::Grid::Error setParam(const char* name, const char* value);
	
	asagi::Grid::Error open(const char* filename);
	
	double getXMin();
	double getYMin();
	double getZMin();
	double getXMax();
	double getYMax();
	double getZMax();
	
	char getByte(double x, double y = 0, double z = 0);
	int getInt(double x, double y = 0, double z = 0);
	long getLong(double x, double y = 0, double z = 0);
	float getFloat(double x, double y = 0, double z = 0);
	double getDouble(double x, double y = 0, double z = 0);
	void getBuf(void* buf, double x, double y = 0, double z = 0);
	
	bool exportPng(const char* filename);

private:
	void getAt(void* buf, types::Type::converter_t converter,
		   double x, double y = 0, double z = 0);
	
	/**
	 * This function is used by {@link exportPng(const char*)},
	 * which only works on floats
	 */
	float getAtFloat(unsigned long x, unsigned long y);
protected:
	MPI_Comm getMPICommunicator()
	{
		return m_container.getMPICommunicator();
	}
	int getMPIRank()
	{
		return m_container.getMPIRank();
	}
	int getMPISize()
	{
		return m_container.getMPISize();
	}
	
	io::NetCdf& getInputFile()
	{
		return *m_inputFile;
	}
	
	types::Type& getType()
	{
		return m_container.getType();
	}
	
	unsigned long getXDim()
	{
		return m_dim[0];
	}
	unsigned long getYDim()
	{
		return m_dim[1];
	}
	unsigned long getZDim()
	{
		return m_dim[2];
	}
	
	unsigned long getBlocksPerNode();
	
	/**
	 * @return The number of values in x direction in each block
	 */
	unsigned long getXBlockSize()
	{
		return m_blockSize[0];
	}
	/**
	 * @return The number of values in y direction in each block
	 */
	unsigned long getYBlockSize()
	{
		return m_blockSize[1];
	}
	/**
	 * @return The number of values in z direction in each block
	 */
	unsigned long getZBlockSize()
	{
		return m_blockSize[2];
	}
	
	/**
	 * @return The number of blocks in the grid
	 */
	unsigned long getBlockCount()
	{
		return blocks[0] * blocks[1] * blocks[2];
	}
	
	/**
	 * Calculates the position of <code>block</code> in the grid
	 */
	void getBlockPos(unsigned long block,
		unsigned long &x, unsigned long &y, unsigned long &z)
	{
		x = block % blocks[0];
		y = (block / blocks[0]) % blocks[1];
		z = block / (blocks[0] * blocks[1]);
	}
	

	/**
	 * @return The block that stores the value at (x, y)
	 */
	unsigned long getBlockByCoords(unsigned long x, unsigned long y,
		unsigned long z)
	{
		return (((z / getZBlockSize()) * blocks[1]
			+ (y / getYBlockSize())) * blocks[0])
			+ (x / getXBlockSize());
	}
	
	virtual asagi::Grid::Error init() = 0;
	
	virtual void getAt(void* buf, types::Type::converter_t converter,
		unsigned long x, unsigned long y = 0, unsigned long z = 0) = 0;
private:
	/** Name of the dimensions: "x", "y" and "z" */
	static const char* DIMENSION_NAMES[3];
private:
	static void h2rgb(float h, unsigned char &red, unsigned char &green,
		unsigned char &blue);
	static double getInvScaling(double scaling);
	static double round(double value);
};

#endif // GRID_H
