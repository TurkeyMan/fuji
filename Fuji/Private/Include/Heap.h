#if !defined(_HEAP_H)
#define _HEAP_H

#include "PtrList.h"

#define MAX_HEAP_COUNT	6
#define MAX_RESOURCES	254
#define MAX_ALLOC_COUNT	2048
#define HEAP_MARKERS	16

enum ResType
{
	RES_Unknown = 0,
	RES_PtrList,
	RES_Texture,
	RES_Model,
	RES_SoundBank,
	RES_IniFile,
	RES_Collision
};

struct MemAlloc
{
	char *pAddress;
	uint32 bytes;

	char *pFilename;
	int lineNumber;
};

class Heap
{
public:
	char *pHeap;
	char *pAllocPointer;
	uint32 heapSize;

	char *markStack[HEAP_MARKERS];
	uint32 markCount;
#if !defined(_RETAIL)
	uint32 markAlloc[HEAP_MARKERS];

	char heapName[40];

	MemAlloc allocList[MAX_ALLOC_COUNT];
	uint32 allocCount;
#endif
};

class Resource
{
public:
	void *pData;
	uint32 bytes;
	uint32 resourceType;
	uint32 reserved;
};

template<class T>
class MFResource
{
public:
	inline T& operator=(Resource *pR)	{ pResource = pR; return(*this); }
	inline T& operator*() const		{ return *(T*)pResource->pData; }
	inline T* operator->() const	{ return (T*)pResource->pData; }
	inline operator T*() const		{ return (T*)pResource->pData; }

	uint32 resID;
};

void Heap_InitModule();
void Heap_DeinitModule();

Heap* CreateHeap(uint32 size, char *name);
void FreeHeap(Heap *pHeap);

Resource* CreateResource(uint32 size, uint32 type);
void ReleaseResource(Resource *pResource);

void MarkHeap(Heap *pHeap);
void ReleaseMark(Heap *pHeap);

void SetCurrentHeap(Heap *pHeap);

#if !defined(_RETAIL)
void *Heap_Alloc(uint32 bytes, char *pFile = __FILE__, uint32 line = __LINE__);
void *Heap_Realloc(void *pMem, uint32 bytes, char *pFile, uint32 line);
#else
void *Heap_Alloc(uint32 bytes);
void *Heap_Realloc(void *pMem, uint32 bytes);
#endif
void Heap_Free(void *pMem);

#if !defined(_RETAIL)
#define Heap_New(T) Managed_New(new T, __FILE__, __LINE__)
template<class T>
T* Managed_New(T *pT, char *pFile, uint32 line);
#else
#define Heap_New(T) Managed_New(new T)
template<class T>
T* Unmanaged_New(T *pT);
#endif
template<class T>
void Heap_Delete(T *pObject);

/*
template<class T>
#if !defined(_RETAIL)
T* Heap_NewArray(int arraySize, char *pFile, uint32 line);
#else
T* Heap_NewArray(int arraySize);
#endif
template<class T>
void Heap_DeleteArray(T *pArray);
*/

extern Heap *pCurrentHeap;

#endif
