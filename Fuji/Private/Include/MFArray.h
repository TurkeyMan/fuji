#if !defined(_MFARRAY_H)
#define _MFARRAY_H

#if defined(_MSC_VER)
#pragma warning(disable: 4345)
#endif

template<class T>
class MFArray
{
public:
	MFArray();
	MFArray(int _count);

	~MFArray();

	T& operator[](int x);
	const T& operator[](int x) const;

	int size() const;
	void resize(int x);
	void clear();

	T& push();
	T& push(T &x);
	T& pop();

	T& front();
	const T& front() const;

	T& back();
	const T& back() const;

	T* getpointer();

protected:
	int count;
	int allocated;
	T *pData;
};

#include "MFArray.inl"

#endif
