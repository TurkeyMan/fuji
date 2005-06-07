//
// Mount Fuji FString Cache
//
// just a quick and simple string cache
//
class MFStringCache
{
public:
	static MFStringCache *Create(uint32 maxSize);
	static void Destroy(MFStringCache *pCache);
	const char *Add(const char *pNewString);

	// get cache for direct read/write
	char *GetCache() { return pMem; }

	// get the actual used size of the cache
	uint32 GetSize() { return used; }
	
	// use this if you have filled the cache yourself
	void SetSize(uint32 _used) { used = _used; }

protected:
	uint32 size;
	uint32 used;
	char *pMem;
};
