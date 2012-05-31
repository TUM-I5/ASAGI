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
 * @version \$Id$
 */

#ifndef BLOCKS_LRU_H
#define BLOCKS_LRU_H

namespace blocks
{
	
/**
 * Implements the clock page replacement algorithm with 2 hands.
 * This is a simplification of least-recently-used.
 */
class LRU
{
private:
	/** Number of "pages" that are available */
	unsigned long m_size;
	
	/** Next page we try */
	unsigned long m_nextPage;
	
	/** Next page we clear the r bit */
	unsigned long m_nextClear;
	
	/** Flag list for second chance */
	bool *m_referenced;
public:
	LRU();
	virtual ~LRU();
	
	void init(unsigned long size, long handDiff = 0);
	
	/**
	 * An element is accesed, the second chance bit is set
	 */
	void access(unsigned long index)
	{
		m_referenced[index] = true;
	}
	
	/**
	 * Get the index of a free "page"
	 */
	unsigned long getFree()
	{
		do { // Last page was accessed -> execute this loop at least once
			// Clear r bit
			m_referenced[m_nextClear] = false;
		
			// Increment both pointer
			m_nextPage = (m_nextPage + 1) % m_size;
			m_nextClear = (m_nextClear + 1) % m_size;
		} while (m_referenced[m_nextPage]);
	
		// We also access this page
		m_referenced[m_nextPage] = true;
	
		return m_nextPage;
	}
};

}

#endif // BLOCKS_LRU_H
