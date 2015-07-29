/**
 * @file
 *  This file is part of ASAGI.
 *
 *  ASAGI is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as
 *  published by the Free Software Foundation, either version 3 of
 *  the License, or  (at your option) any later version.
 *
 *  ASAGI is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with ASAGI.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Diese Datei ist Teil von ASAGI.
 *
 *  ASAGI ist Freie Software: Sie koennen es unter den Bedingungen
 *  der GNU Lesser General Public License, wie von der Free Software
 *  Foundation, Version 3 der Lizenz oder (nach Ihrer Option) jeder
 *  spaeteren veroeffentlichten Version, weiterverbreiten und/oder
 *  modifizieren.
 *
 *  ASAGI wird in der Hoffnung, dass es nuetzlich sein wird, aber
 *  OHNE JEDE GEWAEHELEISTUNG, bereitgestellt; sogar ohne die implizite
 *  Gewaehrleistung der MARKTFAEHIGKEIT oder EIGNUNG FUER EINEN BESTIMMTEN
 *  ZWECK. Siehe die GNU Lesser General Public License fuer weitere Details.
 *
 *  Sie sollten eine Kopie der GNU Lesser General Public License zusammen
 *  mit diesem Programm erhalten haben. Wenn nicht, siehe
 *  <http://www.gnu.org/licenses/>.
 *
 * @copyright 2015 Sebastian Rettenberger <rettenbs@in.tum.de>
 */

#ifndef TRANSFER_MPICACHE_H
#define TRANSFER_MPICACHE_H

#include "asagi.h"

namespace transfer
{

/**
 * A dictionary for cache entries
 */
template<class Allocator>
class MPICache
{
private:
	/** The NUMA domain ID */
	unsigned int m_numaDomainId;

	/** Number of dictionary entries per element */
	unsigned int m_dictEntrySize;

	/** Global offset of the rank */
	unsigned long m_globalOffset;

	/** Local part of the dictionary */
	long* m_dictionary;

protected:
	MPICache()
		: m_numaDomainId(0),
		  m_dictEntrySize(4), // Default (can not be changed at the moment)
		  m_globalOffset(0),
		  m_dictionary(0L)
	{}

public:
	virtual ~MPICache()
	{
		if (m_numaDomainId == 0)
			Allocator::free(m_dictionary);
	}

protected:
	asagi::Grid::Error init(unsigned long globalOffset,
			unsigned long blockCount,
			numa::NumaComm &numaComm)
	{
		m_numaDomainId = numaComm.domainId();

		m_globalOffset = globalOffset;

		// Allocate the memory and broadcast the pointer
		if (m_numaDomainId == 0) {
			Allocator::allocate(
					blockCount * dictEntrySize() * numaComm.totalDomains(),
					m_dictionary);
		}

		asagi::Grid::Error err = numaComm.broadcast(m_dictionary);
		if (err != asagi::Grid::SUCCESS)
			return err;

		// Set everything in the dictionary to undefined
		for (unsigned long i = blockCount * dictEntrySize() * m_numaDomainId;
				i < blockCount * dictEntrySize() * (m_numaDomainId+1); i++)
			m_dictionary[i] = -1;

		return asagi::Grid::SUCCESS;
	}

	/**
	 * Fetch and update block info
	 *
	 * @param dictEntry The dictionary entry
	 * @param offset The offset on this rank (where the block will be stored)
	 * @return A dictionary entry from where the block can be transfered
	 */
	long fetchAndUpdateBlockInfo(long* dictEntry, unsigned long offset)
	{
		unsigned int count;
		for (count = 0; count < m_dictEntrySize; count++)
			if (dictEntry[count] < 0)
				break;

		long entry = -1;
		if (count > 0)
			entry = dictEntry[rand() % count];

		count = std::min(count, m_dictEntrySize-1); // we don't need t move the last entry
													// if the array is full
		for (int i = count-1; i >= 0; i--)
			dictEntry[i+1] = dictEntry[i];

		// New entry
		dictEntry[0] = m_globalOffset + offset;

		return entry;
	}

	/**
	 * Remove the the current rank form the entry
	 *
	 * @param dictEntry The dictionary entry
	 * @param offset The offset on this rank
	 */
	void deleteBlockInfo(long* dictEntry, unsigned long offset)
	{
		long entry = m_globalOffset + offset;

		unsigned int i;
		for (i = 0; i < m_dictEntrySize; i++) {
			if (dictEntry[i] == entry)
				break;
		}

		for (; i < m_dictEntrySize-1; i++)
			dictEntry[i] = dictEntry[i+1];

		dictEntry[m_dictEntrySize-1] = -1;
	}

	/**
	 * @return The number entries per element
	 */
	unsigned int dictEntrySize() const
	{
		return m_dictEntrySize;
	}

	/**
	 * @return A pointer to a specific dictionary entry
	 */
	long* dictionary(unsigned long entry)
	{
		return &m_dictionary[entry * m_dictEntrySize];
	}

	/**
	 * @return A pointer to the complete dictionary
	 */
	const long* dictionary() const
	{
		return m_dictionary;
	}

	/**
	 * @return A pointer to the complete dictionary
	 */
	long* dictionary()
	{
		return m_dictionary;
	}

protected:
	/** Maximum allowed dictionary size */
	const static unsigned int MAX_DICT_SIZE = 8;
};

}

#endif // TRANSFER_MPICACHE_H
