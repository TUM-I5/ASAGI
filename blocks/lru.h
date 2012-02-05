#ifndef BLOCKS_LRU_H
#define BLOCKS_LRU_H

namespace blocks
{
	/**
	 * Implements the Last-recently-used page replacement algorithm
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
		
		void init(unsigned long size);
		
		void access(unsigned long index);
		unsigned long getFree();
	};
};

#endif // BLOCKS_LRU_H
