//
// Mount Fuji FString Cache
//
// just a quick and simple string cache
//
class MFStringCache
{
public:
	static MFStringCache *Create(int maxSize);
	static void Destroy(MFStringCache *pCache);
	const char *Add(const char *pNewString);

	const char *GetCache() { return pMem; }
	int GetSize() { return uint32(Add("")) - (uint32)pMem; }

protected:
	int size;
	char *pMem;
};
