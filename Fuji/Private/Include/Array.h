#if !defined(_ARRAY_H)
#define _ARRAY_H

template<class T>
class Array
{
public:
	inline Array()
	{
		pData = (T*)malloc(sizeof(T) * 16);
		allocated = 16;
		count = 0;
	}

	inline Array(int _count)
	{
		pData = (T*)realloc(pData, sizeof(T) * _count);
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
			while(x >= allocated) allocated *= 2;

			pData = (T*)realloc(pData, sizeof(T) * allocated);
		}

		count = max(count, x+1);

		return pData[x];
	}

	inline int size() const { return count; }

	inline void resize(int x)
	{
		if(x >= allocated)
		{
			while(x >= allocated) allocated *= 2;

			pData = (T*)realloc(pData, sizeof(T) * allocated);
		}

		count = x;
	}

	inline void clear() { count = 0; }

	inline T& push()
	{
		++count;

		if(count >= allocated)
		{
			allocated *= 2;
			pData = (T*)realloc(pData, sizeof(T) * allocated);
		}

		return pData[count-1];
	}

	inline T& push(T &x)
	{
		++count;

		if(count >= allocated)
		{
			allocated *= 2;
			pData = (T*)realloc(pData, sizeof(T) * allocated);
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
