#ifndef FORTRAN_POINTERARRAY_H
#define FORTRAN_POINTERARRAY_H

#include <assert.h>
#ifndef THREADSAFETY
#include <mutex>
#endif // THREADSAFETY
#include <vector>

#define NULL_INDEX -1

namespace fortran
{
	template<class T> class PointerArray
	{
	private:
		std::vector<T*> vec;
#ifndef THREADSAFETY
		std::mutex vec_mutex;
#endif // THREADSAFETY
	public:
		int add(T* const p)
		{
#ifndef THREADSAFETY
			// Lock vector, otherwise the id (return value)
			// gets messed up
			std::lock_guard<std::mutex> lock(vec_mutex);
#endif // THREADSAFETY
			
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

#endif // FORTRAN_POINTERARRAY_H