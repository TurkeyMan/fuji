/**
 * @file MFStringCache.h
 * @brief Provides a quick and simple string cache.
 * @author James Podesta
 * @defgroup MFStringCache String Cache
 * @{
 */

#if !defined(_MFSTRINGCACHE_H)
#define _MFSTRINGCACHE_H

/**
 * @struct MFStringCache
 * Represents a Fuji String Cache.
 */
struct MFStringCache;

/**
 * Create a string cache.
 * Creates a string cache.
 * @param maxSize First byte.
 * @return Returns a handle to the newly created string cache.
 */
MF_API MFStringCache* MFStringCache_Create(size_t maxSize);

/**
 * Destroy a string cache.
 * Destroys a string cache.
 * @param pCache String cache to destroy.
 * @return None.
 */
MF_API void MFStringCache_Destroy(MFStringCache *pCache);

/**
 * Clear a string cache.
 * Clears a string cache.
 * @param pCache String cache to clear.
 * @return None.
 */
MF_API void MFStringCache_Clear(MFStringCache *pCache);

/**
 * Add a string to a string cache.
 * Adds a string to a string cache.
 * @param pCache Target string cache.
 * @param pNewString String to add to the cache.
 * @return Returns a pointer to the string in the string cache.
 */
MF_API const char* MFStringCache_Add(MFStringCache *pCache, const char *pNewString);

/**
 * Get a pointer to the cache memory.
 * Get a pointer to cache memory for direct read/write.
 * @param pCache Target string cache.
 * @return Returns a pointer to the string in the string cache.
 */
MF_API char* MFStringCache_GetCache(MFStringCache *pCache);

/**
 * Get the actual used size of a string cache.
 * Gets the actual used size of a string cache.
 * @param pCache Target string cache.
 * @return Returns the number of bytes currently used.
 */
MF_API size_t MFStringCache_GetSize(MFStringCache *pCache);

/**
 * Set the number of bytes used in a string cache.
 * Sets the number of bytes used in a string cache. Use this if you have filled the cache yourself.
 * @param pCache Target string cache.
 * @param used New number of bytes used.
 * @return None.
 */
MF_API void MFStringCache_SetSize(MFStringCache *pCache, size_t used);

//
// unicode support
//

/**
 * @struct MFWStringCache
 * Represents a Fuji Unicode String Cache.
 */
struct MFWStringCache;

/**
 * Create a unicode string cache.
 * Creates a unicode string cache.
 * @param maxSize First byte.
 * @return Returns a handle to the newly created unicode string cache.
 */
MF_API MFWStringCache* MFWStringCache_Create(size_t maxSize);

/**
 * Destroy a unicode string cache.
 * Destroys a unicode string cache.
 * @param pCache String cache to destroy.
 * @return None.
 */
MF_API void MFWStringCache_Destroy(MFWStringCache *pCache);

/**
 * Add a unicode string to a unicode string cache.
 * Adds a unicode string to a unicode string cache.
 * @param pCache Target unicode string cache.
 * @param pNewString String to add to the cache.
 * @return Returns a pointer to the string in the unicode string cache.
 */
MF_API const wchar_t* MFWStringCache_Add(MFWStringCache *pCache, const wchar_t *pNewString);

/**
 * Get a pointer to the cache memory.
 * Get a pointer to cache memory for direct read/write.
 * @param pCache Target unicode string cache.
 * @return Returns a pointer to the string in the unicode string cache.
 */
MF_API wchar_t* MFWStringCache_GetCache(MFWStringCache *pCache);

/**
 * Get the actual used size of a unicode string cache.
 * Gets the actual used size of a unicode string cache.
 * @param pCache Target unicode string cache.
 * @return Returns the number of bytes currently used.
 */
MF_API size_t MFWStringCache_GetSize(MFWStringCache *pCache);

/**
 * Set the number of bytes used in a unicode string cache.
 * Sets the number of bytes used in a unicode string cache. Use this if you have filled the cache yourself.
 * @param pCache Target unicode string cache.
 * @param used New number of bytes used.
 * @return None.
 */
MF_API void MFWStringCache_SetSize(MFWStringCache *pCache, size_t used);

#endif

/** @} */
