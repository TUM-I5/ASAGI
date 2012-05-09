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
		
		bool *m_referenced;

	public:
		LRU();
		virtual ~LRU();
		
		void init(unsigned long size, long handDiff = 0);
		
		void access(unsigned long index);
		unsigned long getFree();
	};
};

#endif // BLOCKS_LRU_H
