#import "Foundation/Foundation.h"
#import "MFFileSystemNative_IPhone.h"

#ifdef _cplusplus
extern "C" {
#endif // cpp

#include "Fuji.h"

const char *GetCString(NSString *str)
{
	const char *pUTF8 = [str UTF8String];
	return MFStr(pUTF8);
}

const char *GetCurrentDir()
{
	NSString *cur = [[NSFileManager defaultManager] currentDirectoryPath];
	return GetCString(cur);
}

const char *GetUserHome()
{
	NSString * home = NSHomeDirectory();
	return GetCString(home);
}

const char *GetAppHome()
{
	NSBundle * mb = [NSBundle mainBundle];
	NSString * mbp = [mb bundlePath];
	return GetCString(mbp);
}

#ifdef _cplusplus
}
#endif // cpp
