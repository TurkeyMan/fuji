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

protected:
	int size;
	char *pMem;
};
