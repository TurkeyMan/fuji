#if !defined(_HEAP_H)
#define _HEAP_H

template<class T>
class MFPtr
{
	inline operator=(T *pPtr) { pData = pPtr; };
	inline operator*() { return *pData; };
	inline operator->() { return *pData; };

	T *pData;
	uint32 bytes;
};

void *Heap_Alloc(uint32 bytes);
void *Heap_Realloc(uint32 bytes);
void Heap_Free(void *pMem);

void Heap_New();
void Heap_Delete(void *pMem);

void *Heap_NewArray(int arraySize);
void *Heap_RenewArray(int arraySize);
void Heap_DeleteArray(void *pMem);

#endif
