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
	MFArray(int maxItems);
	~MFArray();

	T& operator[](int i);
	const T& operator[](int i) const;

	T& front();
	const T& front() const;

	T& back();
	const T& back() const;

	T& push();
	T& push(const T &item);
	T& pop();

	int reserve(int maxItems);

	int size() const;
	void resize(int length);
	void clear();

	T* getpointer() const;

protected:
	int alloc(int count);

	int count;
	int allocated;
	T *pData;
};

#include "MFArray.inl"

#endif
