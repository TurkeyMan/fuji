
#include "MFHeap.h"

extern void *gEmptyPtrList[2];
extern void *gEmptyPtrListDL[2];

template<class T>
inline void MFPtrList<T>::Init(const char *pGroupName, uint32 maxElements)
{
	MFCALLSTACK;

	if(maxElements==0)
	{
		ppMark = (T**)(&gEmptyPtrList[1]);
	}
	else
	{
		MFHeap_PushGroupName(pGroupName);
		ppMark = (T**)(MFHeap_Alloc((maxElements + 2)*(int)sizeof(T*)));
		MFHeap_PopGroupName();

		*ppMark = 0;

		for(uint32 i = 0; i<maxElements; ++i)
			*(++ppMark) = (T*)(0xdeadbeef);

		*(++ppMark) = 0;
	}

#if !defined(_RETAIL)
	pName = pGroupName;
#endif
}

template<class T>
inline void MFPtrList<T>::Deinit()
{
	MFCALLSTACK;
	MFDebug_Assert(ppMark!=0, "not initialised");

	while(*(--ppMark)!=0) {}

	if(ppMark!=(T**)(&gEmptyPtrList[0]))
		MFHeap_Free(ppMark);

	ppMark = 0;
}

template<class T>
inline T* MFPtrList<T>::Create(T* p)
{
	MFCALLSTACK;
	MFDebug_Assert(p!=0, "invalid parameter");
	MFDebug_Assert(!IsFull(), MFStr("list %s full",pName));

	return *(--ppMark) = p;
}

template<class T>
inline void MFPtrList<T>::Destroy(T* p)
{
	MFCALLSTACK;
	MFDebug_Assert(p!=0, "invalid parameter");

	T **iterator = ppMark;
	while(*iterator!=0)
	{
		if(*iterator==p)
		{
			Destroy(iterator);
			return;
		}
		++iterator;
	}
	MFDebug_Assert(false, MFStr("item not found in list %s",pName));
}

template<class T>
inline T** MFPtrList<T>::Begin()
{
	return ppMark;
}

template<class T>
inline T** MFPtrList<T>::Find(T* p)
{
	MFCALLSTACK;
	MFDebug_Assert(p!=0, "invalid parameter");

	T **iterator = ppMark;
	while(*iterator!=0)
	{
		if(*iterator==p)
		{
			return iterator;
		}
		++iterator;
	}
	return NULL;
}

template<class T>
inline void MFPtrList<T>::Destroy(T** p)
{
	MFCALLSTACK;
	MFDebug_Assert(p!=0, "invalid parameter");
	MFDebug_Assert(!IsEmpty(), MFStr("list %s is empty",pName));

	*p = *(ppMark++);
}

template<class T>
inline void MFPtrList<T>::Clear()
{
	while(*ppMark!=0) ++ppMark;
}

template<class T>
inline int MFPtrList<T>::GetLength()
{
	T** iterator = ppMark;
	while(*iterator!=0) ++iterator;
	return iterator - ppMark;
}

template<class T>
inline int MFPtrList<T>::GetMaxElements()
{
	T** iterator = ppMark;
	while(*(--iterator)!=0);
	return ppMark - iterator - 1 + GetLength();
}

template<class T>
inline bool MFPtrList<T>::IsFull()
{
	return *(ppMark - 1)==0;
}

template<class T>
inline bool MFPtrList<T>::IsEmpty()
{
	return *ppMark==0;
}


template<class T>
inline void MFPtrListDL<T>::Init(const char *pGroupName, uint32 maxElements, uint32 elementSize, void *pMem)
{
	MFCALLSTACK;

	if(maxElements==0)
	{
		MFDebug_Assert(maxElements!=0, "List must have at least 1 element.");
		ppMark = (T**)(&gEmptyPtrListDL[1]);
	}
	else
	{
		ppMark = (T**)MFHeap_Alloc(maxElements*elementSize + (maxElements + 2)*(int)sizeof(T*));

		T* pBegin = (T*)ppMark;
		ppMark = (T**)((char*)pBegin + maxElements*elementSize);
		*ppMark = 0;

		for(uint32 i = 0; i<maxElements; ++i)
			pBegin = (T*)(int(*(++ppMark) = pBegin) + elementSize);

		*(++ppMark) = 0;
	}

#if !defined(_RETAIL)
	pName = pGroupName;
#endif
}

template<class T>
inline void MFPtrListDL<T>::Deinit()
{
	MFCALLSTACK;
	MFDebug_Assert(ppMark!=0, "not initialised"); // stops double deinit's

	T* mem = (T*)(ppMark);  // initialise with a high value
	T** iterator = ppMark;

	while(*(--iterator)!=0)
	{
		if(*iterator<mem) mem = *iterator;
	}

	if(iterator!=(T**)(&gEmptyPtrListDL[0]))
	{
		iterator = ppMark;

		while(*iterator!=0)
		{
			if(*iterator<mem) mem = *iterator;
			++iterator;
		}

		MFHeap_Free(mem);
	}

	ppMark = 0;
}

template<class T>
inline T* MFPtrListDL<T>::Create()
{
	MFDebug_Assert(!IsFull(), MFStr("list %s full",pName));
	return *(--ppMark);
};

template<class T>
inline void MFPtrListDL<T>::Destroy(T* p)
{
	MFCALLSTACK;
	MFDebug_Assert(p!=0, "invalid parameter");

	T **iterator = ppMark;
	while(*iterator!=0)
	{
		if(*iterator==p)
		{
			Destroy(iterator);
			return;
		};
		++iterator;
	};
	MFDebug_Assert(false, MFStr("List %s, item not found",pName));
}

template<class T>
inline T** MFPtrListDL<T>::Begin()
{
	return ppMark;
};

template<class T>
inline T** MFPtrListDL<T>::Find(T* p)
{
	MFCALLSTACK;
	MFDebug_Assert(p!=0, "invalid parameter");

	T **iterator = ppMark;
	while(*iterator!=0)
	{
		if(*iterator==p)
		{
			return iterator;
		};
		++iterator;
	};
	return NULL;
}

template<class T>
inline void MFPtrListDL<T>::Swap(T **p1, T **p2)
{
	T *pT = *p1;
	*p1 = *p2;
	*p2 = pT;
}

template<class T>
inline void MFPtrListDL<T>::Destroy(T** p)
{
	MFDebug_Assert(p!=0, "invalid parameter");
	MFDebug_Assert(!IsEmpty(), MFStr("list %s is empty",pName));
	Swap(ppMark++, p);
}

template<class T>
inline void MFPtrListDL<T>::Clear()
{
	while(*ppMark!=0) ++ppMark;
}

template<class T>
inline int MFPtrListDL<T>::GetLength()
{
	T** iterator = ppMark;
	while(*iterator!=0) ++iterator;
	return iterator - ppMark;
}

template<class T>
inline int MFPtrListDL<T>::GetMaxElements()
{
	T** iterator = ppMark;
	while(*(--iterator)!=0) {}
	return ppMark - iterator - 1 + GetLength();
}

template<class T>
inline bool MFPtrListDL<T>::IsFull()
{
	return *(ppMark - 1)==0;
}

template<class T>
inline bool MFPtrListDL<T>::IsEmpty()
{
	return *ppMark==0;
}

template<class T>
inline T** MFPtrListDL<T>::BeginDead()
{
	return ppMark - 1;
}
