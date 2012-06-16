/**
 * @file
 *  This file is part of ASAGI.
 * 
 *  ASAGI is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  ASAGI is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with ASAGI.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Diese Datei ist Teil von ASAGI.
 *
 *  ASAGI ist Freie Software: Sie koennen es unter den Bedingungen
 *  der GNU General Public License, wie von der Free Software Foundation,
 *  Version 3 der Lizenz oder (nach Ihrer Option) jeder spaeteren
 *  veroeffentlichten Version, weiterverbreiten und/oder modifizieren.
 *
 *  ASAGI wird in der Hoffnung, dass es nuetzlich sein wird, aber
 *  OHNE JEDE GEWAEHELEISTUNG, bereitgestellt; sogar ohne die implizite
 *  Gewaehrleistung der MARKTFAEHIGKEIT oder EIGNUNG FUER EINEN BESTIMMTEN
 *  ZWECK. Siehe die GNU General Public License fuer weitere Details.
 *
 *  Sie sollten eine Kopie der GNU General Public License zusammen mit diesem
 *  Programm erhalten haben. Wenn nicht, siehe <http://www.gnu.org/licenses/>.
 * 
 * @copyright 2012 Sebastian Rettenberger <rettenbs@in.tum.de>
 */

#ifndef GRIDCONTAINER_H
#define GRIDCONTAINER_H

#include <asagi.h>

#include "fortran/pointerarray.h"
#include "types/type.h"

class Grid;

/**
 * A container that stores multiple levels of a grid
 */
class GridContainer : public asagi::Grid
{
public:
	/** Possible positions of the values in a grid */
	enum ValuePos { CELL_CENTERED, VERTEX_CENTERED };
private:
	/** Id of the grid, used for the fortran <-> c interface */
	int m_id;
	
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
	
protected:
	/** Number of levels this grid container has */
	const unsigned int m_levels;
	
	/** Minimum in x dimension (set by subclasses) */
	double m_minX;
	/** Minimum in y dimension (set by subclasses) */
	double m_minY;
	/** Minimum in z dimension (set by subclasses) */
	double m_minZ;
	/** Maximum in x dimension (set by subclasses) */
	double m_maxX;
	/** Maximum in y dimension (set by subclasses) */
	double m_maxY;
	/** Maximum in z dimension (set by subclasses) */
	double m_maxZ;
	
	/** Value Position (cell-centered || vertex-centered) */
	ValuePos m_valuePos;
public:
	GridContainer(Type type, bool isArray = false,
		unsigned int hint = asagi::NO_HINT,
		unsigned int levels = 1);
	GridContainer(unsigned int count,
		unsigned int blockLength[],
		unsigned long displacements[],
		asagi::Grid::Type types[],
		unsigned int hint = NO_HINT, unsigned int levels = 1);
	virtual ~GridContainer();
	
#ifndef ASAGI_NOMPI
	Error setComm(MPI_Comm comm = MPI_COMM_WORLD);
#endif // ASAGI_NOMPI
	virtual Error setParam(const char* name, const char* value,
		unsigned int level = 0);
	/**
	 * The default implementation make sure a communicator is set.
	 * <br>
	 * Subclasses should override this function and call open from the 
	 * parent class before doing anythin.
	 */
	virtual Error open(const char* filename, unsigned int level = 0);
	
	double getXMin() const
	{
		return m_minX;
	}
	double getYMin() const
	{
		return m_minY;
	}
	double getZMin() const
	{
		return m_minZ;
	}
	double getXMax() const
	{
		return m_maxX;
	}
	double getYMax() const
	{
		return m_maxY;
	}
	double getZMax() const
	{
		return m_maxZ;
	}
	
	unsigned int getVarSize() const
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
	
#ifndef ASAGI_NOMPI
	/**
	 * @return The commicator used for all grids of this container
	 */
	MPI_Comm getMPICommunicator() const
	{
		return m_communicator;
	}
#endif // ASAGI_NOMPI
	
	/**
	 * @return The current MPI rank
	 */
	int getMPIRank() const
	{
		return m_mpiRank;
	}
	
	/**
	 * @return The size of the MPI communicator
	 */
	int getMPISize() const
	{
		return m_mpiSize;
	}
	
	/**
	 * @return The type assosiated with this grid
	 */
	types::Type& getType() const
	{
		return *m_type;
	}
	
	/**
	 * @return The position of values in the grid
	 */
	ValuePos getValuePos() const
	{
		return m_valuePos;
	}
	
	/**
	 * Converts the C pointer of the grid to the Fortran identifier
	 * 
	 * @return The unique index of the grid container
	 */
	int c2f() const
	{
		return m_id;
	}
private:
	/** The index -> pointer translation array */
	static fortran::PointerArray<GridContainer> m_pointers;
public:
	/**
	 * Converts a Fortran index to a c/c++ pointer
	 */
	static GridContainer* f2c(int i)
	{
		return m_pointers.get(i);
	}
};

#endif // GRIDCONTAINER_H
