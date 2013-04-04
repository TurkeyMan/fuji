#include "Fuji.h"
#include "MFResource_Internal.h"

#include "MFModule.h"
#include "MFOpenHashTable.h"

typedef MFOpenHashTable<MFResource*> MFResourceHashTable;
static MFResourceHashTable gResourceTable;

static int gResourceCounts[MFRT_Max + 2];

MFInitStatus MFResource_InitModule()
{
	MFZeroMemory(gResourceCounts, sizeof(gResourceCounts));

	gResourceTable.Init(1024, 1024, 1024);

	return MFAIC_Succeeded;
}

void MFResource_DeinitModule()
{
	// scan remaining resources
	//...

	gResourceTable.Deinit();
}

MF_API void MFResource_AddResource(MFResource *pResource)
{
	MFDebug_Assert(gResourceTable.Get(pResource->hash) == NULL, "Resource hash collision!");

	gResourceTable.Add(pResource->hash, pResource);

	++gResourceCounts[pResource->type + 2];
	++gResourceCounts[0];
}

MF_API void MFResource_RemoveResource(MFResource *pResource)
{
	gResourceTable.Destroy(pResource->hash);

	--gResourceCounts[pResource->type + 2];
	--gResourceCounts[0];
}

MF_API MFResource* MFResource_FindResource(uint32 hash)
{
	MFResourceHashTable::Iterator i = gResourceTable.Get(hash);
	if(i)
		return *i;
	return NULL;
}

MF_API MFResourceType MFResource_GetType(MFResource *pResource)
{
	return (MFResourceType)pResource->type;
}

MF_API uint32 MFResource_GetHash(MFResource *pResource)
{
	return pResource->hash;
}

MF_API int MFResource_GetRefCount(MFResource *pResource)
{
	return pResource->refCount;
}

MF_API int MFResource_GetNumResources(MFResourceType type)
{
	return gResourceCounts[type + 2];
}

MF_API MFResourceIterator* MFResource_EnumerateFirst(MFResourceType type)
{
	MFResourceHashTable::Iterator i = gResourceTable.First();
	while(i && type != MFRT_All && (*i)->type != type)
		i = gResourceTable.Next(i);
	return (MFResourceIterator*)i;
}

MF_API MFResourceIterator* MFResource_EnumerateNext(MFResourceIterator *pIterator, MFResourceType type)
{
	MFResourceHashTable::Iterator i = gResourceTable.Next((MFResourceHashTable::Iterator)pIterator);
	while(i && type != MFRT_All && (*i)->type != type)
		i = gResourceTable.Next(i);
	return (MFResourceIterator*)i;
}

MF_API MFResource* MFResource_Get(MFResourceIterator *pIterator)
{
	return pIterator ? *(MFResourceHashTable::Iterator)pIterator : NULL;
}
