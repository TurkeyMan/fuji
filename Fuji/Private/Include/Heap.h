#if !defined(_HEAP_H)
#define _HEAP_H

#include "PtrList.h"

// heap type enum
enum HeapType
{
	HEAP_Static,
	HEAP_Dynamic,
	HEAP_Defrag
};

// system allocation/deallocation
void *malloc_aligned(size_t bytes);
void *realloc_aligned(void *buffer, size_t bytes);
void free_aligned(void *buffer);

// records an allocation
struct MemAlloc
{
	char *pAddress;
	uint32 bytes;

	char *pFilename;
	int lineNumber;
};

// base heap class
class Heap
{
public:
	// Heap Functions
	virtual void Release() = 0;

	virtual void *Alloc(uint32 bytes) = 0;
	virtual void *Realloc(void *pBuffer, uint32 bytes) = 0;
	virtual void Free(void *pBuffer) = 0;

	// Heap Members
	char		*pHeap;
	char		*pAllocPointer;
	uint32		heapSize;
	HeapType	heapType;

#if !defined(_RETAIL)
	char heapName[32];
#endif
};

// static (mark/release) heap
class StaticHeap : public Heap
{
public:
	// Static Heap Functions
	virtual void Release();

	virtual void *Alloc(uint32 bytes);
	virtual void *Realloc(void *pBuffer, uint32 bytes);
	virtual void Free(void *pBuffer);

	// Static Heap Members
	char **markStack;
	uint32 markCount;
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

	Resource *pResource;
};

// heap module init/deinit
void Heap_InitModule();
void Heap_DeinitModule();

// create a heap
Heap* Heap_CreateHeap(uint32 size, HeapType type, char *name);

// get/set the current heap
Heap *Heap_SetCurrentHeap(Heap *pHeap);
Heap *Heap_GetCurrentHeap();

// create a managed resource for dynamic/defragging heaps
Resource* Heap_CreateResource(uint32 size, uint32 type);
void Heap_ReleaseResource(Resource *pResource);

// push/pop a heap marker for static heaps
void Heap_MarkHeap();
void Heap_ReleaseMark();

// push/pop group markers (help keep track of allocation source)
void Heap_PushGroupName(const char *pGroupName);
void Heap_PopGroupName();

// functions to allocate/free memory
#if !defined(_RETAIL)
void *Heap_Alloc(uint32 bytes, char *pFile = __FILE__, uint32 line = __LINE__);
void *Heap_Realloc(void *pMem, uint32 bytes, char *pFile = __FILE__, uint32 line = __LINE__);
#else
void *Heap_Alloc(uint32 bytes);
void *Heap_Realloc(void *pMem, uint32 bytes);
#endif
void Heap_Free(void *pMem);

// for allocating in a temp mem heap
void *Heap_TAlloc(uint32 bytes);
void Heap_TFree(void *pMem);

void Heap_ActivateTempMemOverride(bool activate = true);

#endif
