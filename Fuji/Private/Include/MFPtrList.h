#if !defined(_PTRLIST_H)
#define _PTRLIST_H

#include "MFHeap.h"

extern void *gEmptyPtrList[2];
extern void *gEmptyPtrListDL[2];

template<class T>
class MFPtrList
{
public:
	inline void Init(char* pGroupName, int maxElements);
	inline void Deinit();

	inline T* Create(T* p);
	void Destroy(T* p);

	inline T** Begin()			{ return ppMark; }
	inline T** Find(T* p);
	inline void Destroy(T** p);

	inline void Clear()			{ while(*ppMark!=0) ++ppMark; }

	inline int GetLength()		{ T** iterator = ppMark; while(*iterator!=0) ++iterator; return iterator - ppMark; }
	inline int GetMaxElements()	{ T** iterator = ppMark; while(*(--iterator)!=0); return ppMark - iterator - 1 + GetLength(); }

	inline bool IsFull()		{ return *(ppMark - 1)==0; }
	inline bool IsEmpty()		{ return *ppMark==0; }

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
	inline void Init(char* pGroupName, int maxElements, int elementSize = sizeof(T), void *pMem = NULL);
	inline void Deinit();

	inline T* Create()			{ DBGASSERT(!IsFull(), STR("list %s full",pName)); return *(--ppMark); };
	inline void Destroy(T* p);

	inline T** Begin()			{ return ppMark; };
	inline T** Find(T* p);

	inline void Swap(T **p1, T **p2)
	{
		T *pT = *p1;
		*p1 = *p2;
		*p2 = pT;
	}

	inline void Destroy(T** p)
	{
		DBGASSERT(p!=0, "invalid parameter");
		DBGASSERT(!IsEmpty(), STR("list %s is empty",pName));
		Swap(ppMark++, p);
	};

	inline void Clear()			{ while(*ppMark!=0) ++ppMark; };

	inline int GetLength()		{ T** iterator = ppMark; while(*iterator!=0) ++iterator; return iterator - ppMark; };
	inline int GetMaxElements()	{ T** iterator = ppMark; while(*(--iterator)!=0) {} return ppMark - iterator - 1 + GetLength(); }

	inline bool IsFull()		{ return *(ppMark - 1)==0; };
	inline bool IsEmpty()		{ return *ppMark==0; };

	inline T** BeginDead()		{ return ppMark - 1; };

protected:
	T** ppMark;

#if !defined(_RETAIL)
	char *pName;
#endif
};

#include "MFPtrList.inl"

#endif
