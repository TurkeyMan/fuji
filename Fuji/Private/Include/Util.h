#if !defined(_UTIL_H)
#define _UTIL_H

#define BREAKPOINT { __asm { int 3 }; }

#if defined(_DEBUG)
#define DBGASSERT(condition,str) { static ignore=false; if(!(condition) && !ignore) { dbgAssert(#condition, str, __FILE__, __LINE__); BREAKPOINT; ignore=true; } }
#else
#define DBGASSERT(x, y)
#endif

void dbgAssert(char *pReason, char *pMessage, char *pFile, int line);
int dprintf(char *format, ...);
void LOGD(char *string);
char* STR(char *format, ...);

#endif