#ifndef FORTRANPOINTERARRAY_H
#define FORTRANPOINTERARRAY_H

#include <vector>

#define NULL_INDEX -1

namespace fortran
{
	template<class T> class PointerArray
	{
	private:
		std::vector<T*> vec;
	public:
		int add(T* const p)
		{
			vec.push_back(p);
			return vec.size() - 1;
		}
		
		T* get(int i)
		{
			if (i < 0)
				return 0L;
			if (static_cast<unsigned int>(i) >= vec.size())
				return 0L;
			
			return vec[i];
		}
		
		void remove(int i)
		{
			if (i < 0)
				return;
			if (static_cast<unsigned int>(i) >= vec.size())
				return;
			
			vec[i] = 0L;
		}
	};
};

#endif