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
	
	io::NetCdf *m_inputFile;
	
	/** Total number of elements in x dimension */
	unsigned long m_dimX;
	/** Total number of elements in y dimension */
	unsigned long m_dimY;
	/** Total number of elements in z dimension */
	unsigned long m_dimZ;
	
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
	
	/** Number of blocks in x dimension */
	unsigned long blocksX;
	/** Number of blocks in y dimension */
	unsigned long blocksY;
	/** Number of blocks in z dimension */
	unsigned long blocksZ;
	
	/** Number of values in x dimension in one block */
	unsigned long m_blockSizeX;
	unsigned long m_blockSizeY;
	unsigned long m_blockSizeZ;
public:
	Grid(GridContainer &container);
	virtual ~Grid();
	
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
		return m_dimX;
	}
	unsigned long getYDim()
	{
		return m_dimY;
	}
	unsigned long getZDim()
	{
		return m_dimZ;
	}
	
	unsigned long getBlocksPerNode();
	
	/**
	 * @return The number of values in x direction in each block
	 */
	unsigned long getXBlockSize()
	{
		return m_blockSizeX;
	}
	/**
	 * @return The number of values in y direction in each block
	 */
	unsigned long getYBlockSize()
	{
		return m_blockSizeY;
	}
	/**
	 * @return The number of values in z direction in each block
	 */
	unsigned long getZBlockSize()
	{
		return m_blockSizeZ;
	}
	
	/**
	 * @return The number of blocks in the grid
	 */
	unsigned long getBlockCount()
	{
		return blocksX * blocksY * blocksZ;
	}
	
	/**
	 * Calculates the position of <code>block</code> in the grid
	 */
	void getBlockPos(unsigned long block,
		unsigned long &x, unsigned long &y, unsigned long &z)
	{
		x = block % blocksX;
		y = (block / blocksX) % blocksY;
		z = block / (blocksX * blocksY);
	}
	

	/**
	 * @return The block that stores the value at (x, y)
	 */
	unsigned long getBlockByCoords(unsigned long x, unsigned long y,
		unsigned long z)
	{
		return (((z / m_blockSizeZ) * blocksY
			+ (y / m_blockSizeY)) * blocksX)
			+ (x / m_blockSizeX);
	}
	
	virtual asagi::Grid::Error init() = 0;
	
	virtual void getAt(void* buf, types::Type::converter_t converter,
		unsigned long x, unsigned long y = 0, unsigned long z = 0) = 0;
private:
	static void h2rgb(float h, unsigned char &red, unsigned char &green,
		unsigned char &blue);
	static double getInvScaling(double scaling);
	static double round(double value);
};

#endif // GRID_H