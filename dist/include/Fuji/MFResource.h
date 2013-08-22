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

const int MFRT_All = -2;
const int MFRT_Unknown = -1;
const int MFRT_Max = 32;

extern int MFRT_Texture;
extern int MFRT_Material;
extern int MFRT_VertexDecl;
extern int MFRT_VertexBuffer;
extern int MFRT_IndexBuffer;
extern int MFRT_BlendState;
extern int MFRT_SamplerState;
extern int MFRT_DepthStencilState;
extern int MFRT_RasteriserState;
extern int MFRT_Shader;
extern int MFRT_Effect;
extern int MFRT_ModelTemplate;
extern int MFRT_AnimationTemplate;
extern int MFRT_Sound;
extern int MFRT_Font;

struct MFResource
{
	uint32 hash;
	uint32 type		: 8;
	uint32 refCount	: 24;
	const char *pName;
};

struct MFResourceIterator;

typedef void (DestroyResourceFunc)(MFResource*);

MF_API int MFResource_Register(const char *pResourceName, DestroyResourceFunc *pDestroyFunc);
MF_API const char* MFResource_GetTypeName(int type);

MF_API void MFResource_AddResource(MFResource *pResource, int type, uint32 hash, const char *pName = NULL);
//MF_API void MFResource_RemoveResource(MFResource *pResource);

MF_API MFResource* MFResource_Find(uint32 hash);

MF_API int MFResource_GetType(MFResource *pResource);
MF_API uint32 MFResource_GetHash(MFResource *pResource);
MF_API int MFResource_GetRefCount(MFResource *pResource);
MF_API const char* MFResource_GetName(MFResource *pResource);

MF_API int MFResource_AddRef(MFResource *pResource);
MF_API int MFResource_Release(MFResource *pResource);

MF_API int MFResource_GetNumResources(int type = MFRT_All);

MF_API MFResourceIterator* MFResource_EnumerateFirst(int type = MFRT_All);
MF_API MFResourceIterator* MFResource_EnumerateNext(MFResourceIterator *pIterator, int type = MFRT_All);
MF_API MFResource* MFResource_Get(MFResourceIterator *pIterator);

__forceinline bool MFResource_IsType(MFResource *pResource, int type)
{
	return (int)pResource->type == type;
}


// C++ API
namespace Fuji
{
	class Resource
	{
	public:
		static int RegisterResourceType(const char *pResourceName, DestroyResourceFunc *pDestroyFunc)
		{
			return MFResource_Register(pResourceName, pDestroyFunc);
		}

		inline Resource()						: pResource(NULL) {}
		inline Resource(const Resource &from)	: pResource(from.pResource) { AddRef(); }
		inline Resource(MFResource *pFrom)		: pResource(pFrom) { AddRef(); }
		inline ~Resource()						{ Release(); }

		inline Resource& operator=(const Resource& from)
		{
			Release();
			pResource = from.pResource;
			AddRef();
			return *this;
		}

		inline operator MFResource*()			{ return pResource; }

		inline int RefCount() const				{ return pResource ? pResource->refCount : 0; }
		inline int AddRef()						{ if(pResource) { return ++pResource->refCount; } return 0; }
		inline int Release()					{ int rc = 0; if(pResource) { rc = MFResource_Release(pResource); pResource = NULL; } return rc; }

		inline int ResourceType() const			{ return (int)pResource->type; }
		inline const char *ResourceName() const	{ return pResource->pName; }
		inline uint32 Hash() const				{ return pResource->hash; }

		inline bool IsType(int type) const		{ return (int)pResource->type == type; }

	protected:
		MFResource *pResource;
	};
}

#endif // _MFRESOURCE_H

/** @} */
