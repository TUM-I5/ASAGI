#ifndef FORTRANPOINTERARRAY_H
#define FORTRANPOINTERARRAY_H

#include <pthread.h>
#include <vector>

#define NULL_INDEX -1

namespace fortran
{
	template<class T> class PointerArray
	{
	private:
		std::vector<T*> vec;
		pthread_mutex_t vec_mutex;
	public:
		PointerArray()
		{
			pthread_mutex_init(&vec_mutex, 0L);
		}
		
		virtual ~PointerArray()
		{
			pthread_mutex_destroy(&vec_mutex);
		}
		
		int add(T* const p)
		{
			int id;
			
			// Lock vector, otherwise the id gets messed up
			pthread_mutex_lock(&vec_mutex);
			vec.push_back(p);
			id = vec.size() - 1;
			pthread_mutex_unlock(&vec_mutex);
			
			return id;
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