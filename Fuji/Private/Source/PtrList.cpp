//#include "Common.h"
#include "PtrList.h"

template<typename T>
void PtrList<T>::Init(char* pGroupName, int maxElements)
{
  CALLSTACK("PtrList::Init");

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

#if defined(_MKDEBUG)
  pName = pGroupName;
#endif
}


template<typename T>
void PtrList<T>::Deinit()
{
  CALLSTACK("PtrList::Deinit");
  DBGASSERT(ppMark!=0, "not initialised");

  while(*(--ppMark)!=0) {}
  if(ppMark!=(T**)(&gEmptyPtrList[0])) Heap_Free(ppMark);
  ppMark = 0;
}

template<typename T>
T* PtrList<T>::Create(T* p)
{
  DBGASSERT(p!=0, "invalid parameter");
  DBGASSERT(!IsFull(), STR("list %s full",pName));
  
  return *(--ppMark) = p;
}

template<typename T>
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

template<typename T>
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

template<typename T>
void PtrList<T>::Destroy(T** p)
{
  DBGASSERT(p!=0, "invalid parameter");
  DBGASSERT(!IsEmpty(), STR("list %s is empty",pName));

  *p = *(ppMark++);
}



template<typename T>
void PtrListDL<T>::Init(char* pGroupName, int maxElements, int elementSize)
{
  CALLSTACK("PtrListDL::Init");

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

#if defined(_DBGASSERTS)
  pName = pGroupName;
#endif
}

template<typename T>
void PtrListDL<T>::Deinit()
{
  CALLSTACK("PtrListDL::Deinit");
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

template<typename T>
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

template<typename T>
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
