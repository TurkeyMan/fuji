#if !defined(_PTRLIST_H)
#define _PTRLIST_H

template<class T>
class PtrList
{
public:
  inline void Init(char* pGroupName, int maxElements);
  inline void Deinit();

  inline T* Create(T* p);
  void Destroy(T* p);

  inline T** Begin()          { return ppMark; }
  inline T** Find(T* p);
  inline void Destroy(T** p);

  inline void Clear()    { while(*ppMark!=0) ++ppMark; }

  inline int GetLength()      { T** iterator = ppMark; while(*iterator!=0) ++iterator; return iterator - ppMark; }
  inline int GetMaxElements() { T** iterator = ppMark; while(*(--iterator)!=0); return ppMark - iterator - 1 + GetLength(); }

  inline bool IsFull()        { return *(ppMark - 1)==0; }
  inline bool IsEmpty()       { return *ppMark==0; }

protected:
  T** ppMark;

#if defined(_DEBUG)
  char *pName;
#endif
};

template<class T>
void PtrList<T>::Init(char* pGroupName, int maxElements)
{
  CALLSTACK("PtrList::Init");
/*
  if(maxElements==0)
  {
    ppMark = (T**)(&gEmptyPtrList[1]);
  }
  else
  {
    Heap_PushGroupName( pGroupName );
    ppMark = (T**)(Heap_Alloc((maxElements + 2)*(int)sizeof(T*)));
    Heap_PopGroupName();

    *ppMark = 0;
    for(int i = 0; i<maxElements; ++i) *(++ppMark) = (T*)(0xdeadbeef);
    *(++ppMark) = 0;
  }
*/
#if defined(_MKDEBUG)
  pName = pGroupName;
#endif
}

template<class T>
void PtrList<T>::Deinit()
{
  CALLSTACK("PtrList::Deinit");
  DBGASSERT(ppMark!=0, "not initialised");
/*
  while(*(--ppMark)!=0) {}
  if(ppMark!=(T**)(&gEmptyPtrList[0])) Heap_Free(ppMark);
  ppMark = 0;
*/
}

template<class T>
T* PtrList<T>::Create(T* p)
{
  DBGASSERT(p!=0, "invalid parameter");
  DBGASSERT(!IsFull(), STR("list %s full",pName));
  
  return *(--ppMark) = p;
}

template<class T>
void PtrList<T>::Destroy(T* p)
{
  CALLSTACK("PtrListDL::Destroy");
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
T** PtrList<T>::Find(T* p)
{
  CALLSTACK("PtrListDL::Destroy");
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
void PtrList<T>::Destroy(T** p)
{
  DBGASSERT(p!=0, "invalid parameter");
  DBGASSERT(!IsEmpty(), STR("list %s is empty",pName));

  *p = *(ppMark++);
}



template<class T>
class PtrListDL
{
public:
  inline void Init(char* pGroupName, int maxElements, int elementSize = sizeof(T));
  inline void Deinit();

  inline T* Create() { DBGASSERT(!IsFull(), STR("list %s full",pName)); return *(--ppMark); };
  inline void Destroy(T* p);

  inline T** Begin()          { return ppMark; };
  inline T** Find(T* p);
  inline void Destroy(T** p)  { DBGASSERT(p!=0, "invalid parameter"); DBGASSERT(!IsEmpty(), STR("list %s is empty",pName)); Swap(*(ppMark++), *p); };

  inline void Clear()    { while(*ppMark!=0) ++ppMark; };

  inline int GetLength()      { T** iterator = ppMark; while(*iterator!=0) ++iterator; return iterator - ppMark; };
  inline int GetMaxElements() { T** iterator = ppMark; while(*(--iterator)!=0) {} return ppMark - iterator - 1 + GetLength(); }

  inline bool IsFull()        { return *(ppMark - 1)==0; };
  inline bool IsEmpty()       { return *ppMark==0; };


  inline T** BeginDead()      { return ppMark - 1; };

protected:
  T** ppMark;

#if defined(_DBGASSERTS)
  char *pName;
#endif
};

template<class T>
void PtrListDL<T>::Init(char* pGroupName, int maxElements, int elementSize)
{
  CALLSTACK("PtrListDL::Init");
/*
  if(maxElements==0)
  {
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
*/
#if defined(_DBGASSERTS)
  pName = pGroupName;
#endif
}

template<class T>
void PtrListDL<T>::Deinit()
{
  CALLSTACK("PtrListDL::Deinit");
  DBGASSERT(ppMark!=0, "not initialised"); // stops double deinit's
/*
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
*/
}

template<class T>
void PtrListDL<T>::Destroy(T* p)
{
  CALLSTACK("PtrListDL::Destroy");
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
T** PtrListDL<T>::Find(T* p)
{
  CALLSTACK("PtrListDL::Destroy");
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

#endif
