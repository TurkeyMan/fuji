#if !defined(_ARRAY_H)
#define _ARRAY_H

#pragma warning(disable: 4345)

#include <new>

template<class T>
class Array
{
public:
	inline Array()
	{
		pData = (T*)malloc(sizeof(T) * 16);
		for(int a=0; a<16; a++) new(&pData[a]) T();
		allocated = 16;
		count = 0;
	}

	inline Array(int _count)
	{
		pData = (T*)malloc(sizeof(T) * _count);
		for(int a=0; a<_count; a++) new(&pData[a]) T();
		allocated = count = _count;
	}

	inline ~Array()
	{
		free(pData);
	}

	inline T& operator[](int x)
	{
		if(x >= allocated)
		{
			int oldAlloc = allocated;
			while(x >= allocated) allocated *= 2;

			pData = (T*)realloc(pData, sizeof(T) * allocated);
			for(int a=oldAlloc; a<allocated; a++) new(&pData[a]) T();
		}

		count = Max(count, x+1);

		return pData[x];
	}

	inline int size() const { return count; }

	inline void resize(int x)
	{
		if(x >= allocated)
		{
			int oldAlloc = allocated;
			while(x >= allocated) allocated *= 2;

			pData = (T*)realloc(pData, sizeof(T) * allocated);
			for(int a=oldAlloc; a<allocated; a++) new(&pData[a]) T();
		}

		count = x;
	}

	inline void clear() { count = 0; }

	inline T& push()
	{
		++count;

		if(count >= allocated)
		{
			int oldAlloc = allocated;
			allocated *= 2;
			pData = (T*)realloc(pData, sizeof(T) * allocated);
			for(int a=oldAlloc; a<allocated; a++) new(&pData[a]) T();
		}

		return pData[count-1];
	}

	inline T& push(T &x)
	{
		++count;

		if(count >= allocated)
		{
			int oldAlloc = allocated;
			allocated *= 2;
			pData = (T*)realloc(pData, sizeof(T) * allocated);
			for(int a=oldAlloc; a<allocated; a++) new(&pData[a]) T();
		}

		pData[count-1] = x;

		return pData[count-1];
	}

	inline T& pop()
	{
		--count;

		return pData[count-1];
	}

	inline T& front()
	{
		return pData[0];
	}

	inline T& back()
	{
		return pData[count-1];
	}

	int count;
	int allocated;
	T *pData;
};

#endif
