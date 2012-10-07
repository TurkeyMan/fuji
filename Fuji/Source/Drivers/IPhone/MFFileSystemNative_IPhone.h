#ifndef _APPLE_STRING_H_
#define _APPLE_STRING_H_

//#ifdef _cplusplus
//extern "C" {
//#endif // cpp

const char *GetCString(NSString *str);

const char *GetCurrentDir(char *pPath);
const char *GetUserHome(char *pPath);
const char *GetAppHome(char *pPath);

//#ifdef _cplusplus
//}
//#endif // cpp

#endif // _APPLE_STRING_H_
