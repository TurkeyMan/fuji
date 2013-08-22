#if !defined(_MFALLOCATOR_H)
#define _MFALLOCATOR_H

#include "MFHeap.h"
#include "MFObjectPool.h"

#include <vector>

template<class T>
class MFAllocator : public std::allocator<T>
{
public:
	pointer allocate(size_type n, allocator<void>::const_pointer hint = 0)
	{
		// TODO: what to do with 'hint'?
		return (pointer)MFHeap_Alloc(sizeof(T)*n);
	}

	void deallocate(pointer p, size_type n)
	{
		MFHeap_Free(p);
	}
};

template<class T, size_t PoolSize, size_t GrowSize = 0>
class MFPoolAllocator : public std::allocator<T>
{
public:
	MFPoolAllocator()
	: pool(sizeof(T), PoolSize, GrowSize)
	{}

	pointer allocate(size_type n, allocator<void>::const_pointer hint = 0)
	{
		// TODO: what to do with 'hint'?
		return (pointer)pool.Alloc();
	}

	void deallocate(pointer p, size_type n)
	{
		Ppool.Free(p);
	}

private:
	MFObjectPool pool;
};

#endif
