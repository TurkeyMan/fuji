#if !defined(_PTRLIST_H)
#define _PTRLIST_H

template<class T>
class MFPtrList
{
public:
	void Init(char* pGroupName, int maxElements);
	void Deinit();

	T* Create(T* p);
	void Destroy(T* p);

	T** Begin();
	T** Find(T* p);
	void Destroy(T** p);

	void Clear();

	int GetLength();
	int GetMaxElements();

	bool IsFull();
	bool IsEmpty();

protected:
	T** ppMark;

#if !defined(_RETAIL)
	char *pName;
#endif
};


template<class T>
class MFPtrListDL
{
public:
	void Init(char* pGroupName, int maxElements, int elementSize = sizeof(T), void *pMem = NULL);
	void Deinit();

	T* Create();
	void Destroy(T* p);

	T** Begin();
	T** Find(T* p);

	void Swap(T **p1, T **p2);

	void Destroy(T** p);

	void Clear();

	int GetLength();
	int GetMaxElements();

	bool IsFull();
	bool IsEmpty();

	T** BeginDead();

protected:
	T** ppMark;

#if !defined(_RETAIL)
	char *pName;
#endif
};

#include "MFPtrList.inl"

#endif
