#if !defined(_HEAP_H)
#define _HEAP_H

#define MAX_ALLOC_COUNT 2048
#define HEAP_MARKERS 16

struct MemAlloc
{
	void *pAddress;
	uint32 bytes;

	char *pFilename;
	int lineNumber;
};

class Heap
{
public:
	void *pHeap;
	void *pAllocPointer;
	uint32 heapSize;

	void *markStack[HEAP_MARKERS];
	uint32 markCount;
#if defined(_DEBUG)
	uint32 markAlloc[HEAP_MARKERS];

	char heapName[40];

	MemAlloc allocList[MAX_ALLOC_COUNT];
	uint32 allocCount;
#endif
};

template<class T>
class MFResource
{
public:
	inline operator=(T *pPtr) { pData = pPtr; };
	inline T& operator*() { return *pData; };
	inline T* operator->() { return pData; };

	T *pData;
	uint32 bytes;
};

void Heap_InitModule();
void Heap_DeinitModule();

void CreateHeap(Heap *pHeap, uint32 size, char *name);
void FreeHeap(Heap *pHeap);

void MarkHeap(Heap *pHeap);
void ReleaseMark(Heap *pHeap);

void SetCurrentHeap(Heap *pHeap);

#if defined(_DEBUG)
#define Heap_Alloc(x) Managed_Alloc(x, __FILE__, __LINE__)
void *Managed_Alloc(uint32 bytes, char *pFile, uint32 line);
//void *Managed_Realloc(uint32 bytes, char *pFile, uint32 line);
#else
#define Heap_Alloc(x) Unmanaged_Alloc(x)
void *Unmanaged_Alloc(uint32 bytes);
//void *Unmanaged_Realloc(uint32 bytes);
#endif
void Heap_Free(void *pMem);

template<class T>
#if defined(_DEBUG)
T* Heap_New(char *pFile, uint32 line);
#else
T* Heap_New();
#endif
template<class T>
void Heap_Delete(T *pObject);

template<class T>
#if defined(_DEBUG)
T* Heap_NewArray(int arraySize, char *pFile, uint32 line);
#else
T* Heap_NewArray(int arraySize);
#endif
template<class T>
void Heap_DeleteArray(T *pArray);

extern Heap *pCurrentHeap;

#endif
