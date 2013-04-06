/**
 * @file MFResource.h
 * @brief Fuji resource management.
 * @author Manu Evans
 * @defgroup MFResource Resource Management
 * @{
 */

#pragma once
#if !defined(_MFRESOURCE_H)
#define _MFRESOURCE_H

enum MFResourceType
{
	MFRT_All = -2,
	MFRT_Unknown = -1,

	MFRT_Texture = 0,
	MFRT_Material,
	MFRT_VertexDecl,
	MFRT_VertexBuffer,
	MFRT_IndexBuffer,
	MFRT_BlendState,
	MFRT_SamplerState,
	MFRT_DepthStencilState,
	MFRT_RasteriserState,
	MFRT_Shader,
	MFRT_Effect,
	MFRT_Model,
	MFRT_Animation,
	MFRT_Sound,
	MFRT_Font,

	MFRT_User,

	MFRT_Max = 32,

	MFRT_UserCount = MFRT_Max - MFRT_User,

	MFRT_ForceInt = 0x7FFFFFFF
};

struct MFResource
{
	uint32 hash;
	int16 type;
	int16 refCount;
	const char *pName;
};

struct MFResourceIterator;

MF_API void MFResource_AddResource(MFResource *pResource);
MF_API void MFResource_RemoveResource(MFResource *pResource);

MF_API MFResource* MFResource_FindResource(uint32 hash);

MF_API MFResourceType MFResource_GetType(MFResource *pResource);
MF_API uint32 MFResource_GetHash(MFResource *pResource);
MF_API int MFResource_GetRefCount(MFResource *pResource);

MF_API int MFResource_GetNumResources(MFResourceType type = MFRT_All);

MF_API MFResourceIterator* MFResource_EnumerateFirst(MFResourceType type = MFRT_All);
MF_API MFResourceIterator* MFResource_EnumerateNext(MFResourceIterator *pIterator, MFResourceType type = MFRT_All);
MF_API MFResource* MFResource_Get(MFResourceIterator *pIterator);

__forceinline bool MFResource_IsType(MFResource *pResource, MFResourceType type)
{
	return MFResource_GetType(pResource) == type;
}

#endif // _MFRESOURCE_H

/** @} */
