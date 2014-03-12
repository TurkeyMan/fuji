#include "Fuji_Internal.h"
#include "MFResource_Internal.h"

#include "MFModule.h"
#include "MFOpenHashTable.h"

struct ResourceType
{
	const char *pName;
	DestroyResourceFunction *pDestroyFunc;
};

typedef MFOpenHashTable<MFResource*> MFResourceHashTable;
static MFResourceHashTable gResourceTable;

static ResourceType gResourceTypes[MFRT_Max];
static int gResourceCounts[MFRT_Max + 2];
static int gNumResourceTypes = 0;

int MFRT_Texture = -1;
int MFRT_Material = -1;
int MFRT_RenderTarget = -1;
int MFRT_VertexDecl = -1;
int MFRT_VertexBuffer = -1;
int MFRT_IndexBuffer = -1;
int MFRT_BlendState = -1;
int MFRT_SamplerState = -1;
int MFRT_DepthStencilState = -1;
int MFRT_RasteriserState = -1;
int MFRT_Shader = -1;
int MFRT_Effect = -1;
int MFRT_ModelTemplate = -1;
int MFRT_AnimationTemplate = -1;
int MFRT_Sound = -1;
int MFRT_Font = -1;

MFInitStatus MFResource_InitModule(int moduleId, bool bPerformInitialisation)
{
	MFZeroMemory(gResourceCounts, sizeof(gResourceCounts));

	gResourceTable.Init(1024, 1024, 1024);

	return MFIS_Succeeded;
}

void MFResource_DeinitModule()
{
	// scan remaining resources
	//...

	gResourceTable.Deinit();
}

MF_API int MFResource_Register(const char *pResourceName, DestroyResourceFunction *pDestroyFunc)
{
	int id = gNumResourceTypes++;
	gResourceTypes[id].pName = pResourceName;
	gResourceTypes[id].pDestroyFunc = pDestroyFunc;
	return id;
}

MF_API const char* MFResource_GetTypeName(int type)
{
	return gResourceTypes[type].pName;
}

MF_API void MFResource_AddResource(MFResource *pResource, int type, uint32 hash, const char *pName)
{
	MFDebug_Assert(gResourceTable.Get(hash) == NULL, "Resource hash collision!");

	pResource->type = type;
	pResource->hash = hash;
	pResource->refCount = 1;
	pResource->pName = pName;

	gResourceTable.Add(pResource->hash, pResource);

	++gResourceCounts[pResource->type + 2];
	++gResourceCounts[0];
}

//MF_API void MFResource_RemoveResource(MFResource *pResource)
static void MFResource_RemoveResource(MFResource *pResource)
{
	gResourceTable.Destroy(pResource->hash);

	--gResourceCounts[pResource->type + 2];
	--gResourceCounts[0];
}

MF_API MFResource* MFResource_Find(uint32 hash)
{
	MFResourceHashTable::Iterator i = gResourceTable.Get(hash);
	if(i)
	{
		MFResource *pRes = *i;
		++pRes->refCount;
		return pRes;
	}
	return NULL;
}

MF_API int MFResource_GetType(MFResource *pResource)
{
	return (int)pResource->type;
}

MF_API uint32 MFResource_GetHash(MFResource *pResource)
{
	return pResource->hash;
}

MF_API int MFResource_GetRefCount(MFResource *pResource)
{
	return pResource->refCount;
}

MF_API const char* MFResource_GetName(MFResource *pResource)
{
	return pResource->pName;
}

MF_API int MFResource_AddRef(MFResource *pResource)
{
	return ++pResource->refCount;
}

MF_API int MFResource_Release(MFResource *pResource)
{
	int rc = --pResource->refCount;
	if(rc == 0)
	{
		MFResource_RemoveResource(pResource);
		gResourceTypes[pResource->type].pDestroyFunc(pResource);
	}
	return rc;
}

MF_API int MFResource_GetNumResources(int type)
{
	return gResourceCounts[type + 2];
}

MF_API MFResourceIterator* MFResource_EnumerateFirst(int type)
{
	MFResourceHashTable::Iterator i = gResourceTable.First();
	while(i && type != MFRT_All && (int)(*i)->type != type)
		i = gResourceTable.Next(i);
	return (MFResourceIterator*)i;
}

MF_API MFResourceIterator* MFResource_EnumerateNext(MFResourceIterator *pIterator, int type)
{
	MFResourceHashTable::Iterator i = gResourceTable.Next((MFResourceHashTable::Iterator)pIterator);
	while(i && type != MFRT_All && (int)(*i)->type != type)
		i = gResourceTable.Next(i);
	return (MFResourceIterator*)i;
}

MF_API MFResource* MFResource_Get(MFResourceIterator *pIterator)
{
	return pIterator ? *(MFResourceHashTable::Iterator)pIterator : NULL;
}
