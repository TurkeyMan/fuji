#if !defined(_UTIL_H)
#define _UTIL_H

#define HARD_ASSERTS

class Vector3;

#define BREAKPOINT { __asm { int 3 }; }

#if defined(_DEBUG)
	#if defined(HARD_ASSERTS)
		#define DBGASSERT(condition,str) { static ignore=false; if(!(condition) && !ignore) { hardAssert(#condition, str, __FILE__, __LINE__); BREAKPOINT; ignore=true; } }
	#else
		#define DBGASSERT(condition,str) { static ignore=false; if(!(condition) && !ignore) { dbgAssert(#condition, str, __FILE__, __LINE__); BREAKPOINT; ignore=true; } }
	#endif
#else
#define DBGASSERT(x, y)
#endif

void dbgAssert(const char *pReason, const char *pMessage, const char *pFile, int line);
void hardAssert(const char *pReason, const char *pMessage, const char *pFile, int line);
int dprintf(const char *format, ...);
void LOGD(const char *string);
char* STR(const char *format, ...);

uint32 Rand();
float RandomUnit();
float RandomRange(float min, float max);
Vector3 RandomVector();

#endif
