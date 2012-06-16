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

#ifndef ADAPTIVEGRIDCONTAINER_H
#define ADAPTIVEGRIDCONTAINER_H

#include "gridcontainer.h"

class Grid;

/**
 * @brief An adaptive container that can store multiple grids of the same level
 * 
 * The container allows to store (few) large blocks of each level.
 */
class AdaptiveGridContainer : public GridContainer
{
private:
	/** Optimazation hint */
	const int m_hint;
	
	/** Grid(-parts) */
	std::vector< ::Grid*>* m_grids;
	
	/** Next unique id for a grid */
	unsigned int m_ids;
	
#ifdef THREADSAFETY
	/** Lock the open function ... */
	std::mutex m_mutex;
#endif // THREADSAFETY
	
public:
	AdaptiveGridContainer(Type type, bool isArray = false,
		unsigned int hint = asagi::NO_HINT,
		unsigned int level = 1);
	AdaptiveGridContainer(unsigned int count,
		unsigned int blockLength[],
		unsigned long displacements[],
		asagi::Grid::Type types[],
		unsigned int hint = NO_HINT, unsigned int levels = 1);
	virtual ~AdaptiveGridContainer();
	
	Error open(const char* filename, unsigned int level = 0);
	
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
		
private:
	::Grid* getGrid(double x, double y = 0, double z = 0,
		unsigned int level = 0);
};

#endif // ADAPTIVEGRIDCONTAINER_H
