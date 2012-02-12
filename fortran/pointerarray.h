#ifndef FORTRANPOINTERARRAY_H
#define FORTRANPOINTERARRAY_H

#include <assert.h>
#include <mutex>
#include <vector>

#define NULL_INDEX -1

namespace fortran
{
	template<class T> class PointerArray
	{
	private:
		std::vector<T*> vec;
		std::mutex vec_mutex;
	public:
		int add(T* const p)
		{
			// Lock vector, otherwise the id (return value)
			// gets messed up
			std::lock_guard<std::mutex> lock(vec_mutex);
			
			vec.push_back(p);
			return vec.size() - 1;
		}
		
		T* get(int i)
		{
			assert(i >= 0 &&
				static_cast<unsigned int>(i) < vec.size());
			
			return vec[i];
		}
		
		void remove(int i)
		{
			assert(i >= 0 &&
				static_cast<unsigned int>(i) < vec.size());
			
			vec[i] = 0L;
		}
	};
};

#endif