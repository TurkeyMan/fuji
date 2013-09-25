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
extern int MFRT_RenderTarget;
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


/**
 * Base class for a resource.
 * The base class of any fuji resource.
 */
struct MFResource
{
	uint32 hash;			/**< Resource hash. */
	uint32 type		: 8;	/**< Type id. */
	uint32 refCount	: 24;	/**< Reference count. */
	const char *pName;		/**< Resource name. */
};

/**
 * @struct MFResourceIterator
 * Represents a Fuji resource iterator.
 */
struct MFResourceIterator;

/**
 * Fuji resource destruction callback function prototype.
 */
typedef void (DestroyResourceFunction)(MFResource*);

/**
 * Register a recource type.
 * Registers a new resource type.
 * @param pResourceName The resources type name.
 * @param pDestroyFunc A destroy function, called when the resource ref-count reaches zero.
 * @return An id for the registered resource.
 */
MF_API int MFResource_Register(const char *pResourceName, DestroyResourceFunction *pDestroyFunc);

/**
 * Get the name of a resource type.
 * Gets the name of a resource type.
 * @param type Resource id.
 * @return Returns the type name for the given resource id.
 */
MF_API const char* MFResource_GetTypeName(int type);

/**
 * Add a resource.
 * Adds a resource to the resource manager.
 * @param pResource Resource to add.
 * @param type Type id of the resource being added.
 * @param hash The resources hash.
 * @param pName Optional name for the resource.
 * @return None.
 */
MF_API void MFResource_AddResource(MFResource *pResource, int type, uint32 hash, const char *pName = NULL);

/**
 * Find a resource instance.
 * Finds a resource instance from it's hash.
 * @param hash Resource hash.
 * @return The resource instance, or NULL if it was not found.
 */
MF_API MFResource* MFResource_Find(uint32 hash);

/**
 * Get the type of a resource.
 * Gets the type of the given resorce.
 * @param pResource Pointer to a resource.
 * @return The type id of the resource.
 */
MF_API int MFResource_GetType(MFResource *pResource);

/**
 * Get the hash of a resource.
 * Gets the hash for the given resorce.
 * @param pResource Pointer to a resource.
 * @return The resources hash.
 */
MF_API uint32 MFResource_GetHash(MFResource *pResource);

/**
 * Get the resource reference count.
 * Gets the reference count of the given resorce.
 * @param pResource Pointer to a resource.
 * @return The reference count.
 */
MF_API int MFResource_GetRefCount(MFResource *pResource);

/**
 * Get the resource name.
 * Gets the name of the given resorce.
 * @param pResource Pointer to a resource.
 * @return The resources name, or NULL if no name was given.
 */
MF_API const char* MFResource_GetName(MFResource *pResource);

/**
 * Add a reference to a resource.
 * Increment the reference count of the given resource.
 * @param pResource Pointer to a resource.
 * @return The new reference count.
 */
MF_API int MFResource_AddRef(MFResource *pResource);

/**
 * Release a reference to a resource.
 * Decrement the reference count of the given resource. If the reference count reaches 0, the resource is destroyed.
 * @param pResource Pointer to a resource.
 * @return The new reference count.
 */
MF_API int MFResource_Release(MFResource *pResource);

/**
 * Get number of allocated resources.
 * Gets the number of allocated resources of the given resource type.
 * @param type Resource type id to be counted, or MFRT_All to count all resources.
 * @return The number of resources allocated.
 */
MF_API int MFResource_GetNumResources(int type = MFRT_All);

/**
 * Begin enumeration of resource instances.
 * Enumerate the first instance of the given resource type.
 * @param type Resource type to enumerate.
 * @return A resource iterator, or NULL if no resources of that type exist.
 */
MF_API MFResourceIterator* MFResource_EnumerateFirst(int type = MFRT_All);

/**
 * Get the next resource instance.
 * Gets the next instance of the given resource type.
 * @param pIterator An active resource iterator.
 * @param type Resource type to enumerate.
 * @return A resource iterator pointing to the next resource, or NULL if there are no more resources.
 */
MF_API MFResourceIterator* MFResource_EnumerateNext(MFResourceIterator *pIterator, int type = MFRT_All);

/**
 * Get a resource instance.
 * Gets the resource instance from a resource iterator.
 * @param pIterator An active resource iterator.
 * @return Pointer to the iterator's resource instance.
 */
MF_API MFResource* MFResource_Get(MFResourceIterator *pIterator);

/**
 * Test if a resource is of a type.
 * Tests if a resource is of the given type.
 * @param pResource Pointer to a resource.
 * @param type Resource type id.
 * @return Returns true if \a pResource is of the given \a type.
 */
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
		static int RegisterResourceType(const char *pResourceName, DestroyResourceFunction *pDestroyFunc)
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
