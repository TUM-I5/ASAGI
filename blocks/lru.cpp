#include "lru.h"

using namespace blocks;

LRU::LRU()
{
	m_referenced = 0L;
}

LRU::~LRU()
{
	delete [] m_referenced;
}

void LRU::init(unsigned long size, long handDiff)
{
	m_size = size;
	m_nextPage = size - 1; // Some magic so getFree() works
	if (handDiff < 0)
		// Some default value
		m_nextClear = size / 2;
	else
		m_nextClear = handDiff;
	
	m_referenced = new bool[size];
	for (unsigned long i = 0; i < m_nextClear; i++)
		m_referenced[i] = false;
}

void LRU::access(unsigned long index)
{
	m_referenced[index] = true;
}

unsigned long LRU::getFree()
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