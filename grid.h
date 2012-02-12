#ifndef GRID_H
#define GRID_H

#include <asagi.h>

#include "fortran/pointerarray.h"
#include "types/type.h"

namespace io
{
	class NetCdf;
}

class Grid : public asagi::Grid
{
private:
	/** Id of the grid, used for the fortran <-> c interface */
	int id;
	
	/** The communicator we use */
	MPI_Comm communicator;
	/** Rank of this process */
	int m_mpiRank;
	/** Size of the MPI communicator */
	int m_mpiSize;
	
	io::NetCdf *m_inputFile;
	
	/**
	 * The type of values we save in the grid.
	 * This class implements all type specific operations.
	 */
	types::Type *m_type;
	
	/** Total number of elements in x dimension */
	unsigned long dimX;
	/** Total number of elements in y dimension */
	unsigned long dimY;
	
	double offsetX;
	double offsetY;
	
	double scalingX;
	double scalingY;
	
	/** Number of blocks in x dimension */
	unsigned long blocksX;
	/** Number of blocks in y dimension */
	unsigned long blocksY;
public:
	Grid(Type type = FLOAT);
	virtual ~Grid();
	
	bool open(const char* filename, MPI_Comm comm = MPI_COMM_WORLD);
	
	double getXMin();
	double getYMin();
	double getXMax();
	double getYMax();
	
	unsigned int getVarSize();
	
	char getByte(double x, double y);
	int getInt(double x, double y);
	long getLong(double x, double y);
	float getFloat(double x, double y);
	double getDouble(double x, double y);
	void getBuf(double x, double y, void* buf);
	
	bool exportPng(const char* filename);
	
	// These are not part of the offical interface
	int c2f();
private:
	void getAt(double x, double y, void* buf,
		types::Type::converter_t converter);
	
	/**
	 * This function is used by {@link exportPng(const char*)},
	 * which only works on floats
	 */
	float getAtFloat(unsigned long x, unsigned long y);
protected:
	MPI_Comm getMPICommunicator();
	int getMPIRank();
	int getMPISize();
	
	io::NetCdf* getInputFile();
	
	types::Type* getType();
	
	unsigned long getXDim();
	unsigned long getYDim();
	
	double getXOffset();
	double getYOffset();
	
	double getXScaling();
	double getYScaling();
	
	unsigned long getBlocksPerNode();
	
	unsigned long getXBlockSize();
	unsigned long getYBlockSize();
	unsigned long getBlockCount();
	
	void getBlockPos(unsigned long block,
		unsigned long &x, unsigned long &y);
	
	unsigned long getBlockByCoords(unsigned long x, unsigned long y);
	
	virtual bool init() = 0;
	
	virtual void getAt(unsigned long x, unsigned long y, void* buf,
		types::Type::converter_t converter) = 0;
private:
	static fortran::PointerArray<Grid> pointers;
private:
	static void h2rgb(float h, unsigned char &red, unsigned char &green, unsigned char &blue);
protected:
	static double round(double value);
public:
	static Grid* f2c(int i);
};

#endif