
template<class T>
void MFPtrList<T>::Init(char* pGroupName, int maxElements)
{
	CALLSTACK;

	if(maxElements==0)
	{
		ppMark = (T**)(&gEmptyPtrList[1]);
	}
	else
	{
		Heap_PushGroupName(pGroupName);
		ppMark = (T**)(Heap_Alloc((maxElements + 2)*(int)sizeof(T*)));
		Heap_PopGroupName();

		*ppMark = 0;
		for(int i = 0; i<maxElements; ++i) *(++ppMark) = (T*)(0xdeadbeef);
		*(++ppMark) = 0;
	}

#if !defined(_RETAIL)
	pName = pGroupName;
#endif
}

template<class T>
void MFPtrList<T>::Deinit()
{
	CALLSTACK;
	DBGASSERT(ppMark!=0, "not initialised");

	while(*(--ppMark)!=0) {}
	if(ppMark!=(T**)(&gEmptyPtrList[0])) Heap_Free(ppMark);
	ppMark = 0;
}

template<class T>
T* MFPtrList<T>::Create(T* p)
{
	CALLSTACK;
	DBGASSERT(p!=0, "invalid parameter");
	DBGASSERT(!IsFull(), STR("list %s full",pName));

	return *(--ppMark) = p;
}

template<class T>
void MFPtrList<T>::Destroy(T* p)
{
	CALLSTACK;
	DBGASSERT(p!=0, "invalid parameter");

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
	DBGASSERT(false, STR("item not found in list %s",pName));
}

template<class T>
T** MFPtrList<T>::Find(T* p)
{
	CALLSTACK;
	DBGASSERT(p!=0, "invalid parameter");

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
void MFPtrList<T>::Destroy(T** p)
{
	CALLSTACK;
	DBGASSERT(p!=0, "invalid parameter");
	DBGASSERT(!IsEmpty(), STR("list %s is empty",pName));

	*p = *(ppMark++);
}

template<class T>
void MFPtrListDL<T>::Init(char* pGroupName, int maxElements, int elementSize, void *pMem)
{
	CALLSTACK;

	if(maxElements==0)
	{
		DBGASSERT(maxElements!=0, "List must have at least 1 element.");
		ppMark = (T**)(&gEmptyPtrListDL[1]);
	}
	else
	{
		ppMark = (T**)Heap_Alloc(maxElements*elementSize + (maxElements + 2)*(int)sizeof(T*));

		T* pBegin = (T*)(ppMark);
		*(ppMark = (T**)(int(pBegin) + maxElements*elementSize)) = 0;
		for(int i = 0; i<maxElements; ++i) pBegin = (T*)(int(*(++ppMark) = pBegin) + elementSize);
		*(++ppMark) = 0;
	}

#if !defined(_RETAIL)
	pName = pGroupName;
#endif
}

template<class T>
void MFPtrListDL<T>::Deinit()
{
	CALLSTACK;
	DBGASSERT(ppMark!=0, "not initialised"); // stops double deinit's

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
		};
		Heap_Free(mem);
	}
	ppMark = 0;
}

template<class T>
void MFPtrListDL<T>::Destroy(T* p)
{
	CALLSTACK;
	DBGASSERT(p!=0, "invalid parameter");

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
	DBGASSERT(false, STR("List %s, item not found",pName));
}

template<class T>
T** MFPtrListDL<T>::Find(T* p)
{
	CALLSTACK;
	DBGASSERT(p!=0, "invalid parameter");

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
