#if !defined(_PTRLIST_H)
#define _PTRLIST_H

template<typename T>
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


template<typename T>
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

#endif
