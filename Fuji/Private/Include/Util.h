#if !defined(_UTIL_H)
#define _UTIL_H

#if defined(_XBOX)
	#define HARD_ASSERTS
#endif

class Vector3;

#define BREAKPOINT { __asm { int 3 }; }

#if !defined(_RETAIL)
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
char* STRn(const char *source, int n);

uint32 Rand();
float RandomUnit();
float RandomRange(float min, float max);
Vector3 RandomVector();

inline bool IsWhite(char c)
{
	return c==' ' || c=='\t';
}

inline bool IsAlpha(char c)
{
	return (c>='a' && c<='z') || (c>='A' && c<='Z');
}

inline bool IsNumeric(char c)
{
	return c>='0' && c<='9';
}

inline bool IsAlphaNumeric(char c)
{
	return (c>='a' && c<='z') || (c>='A' && c<='Z') || (c>='0' && c<='9') || (c=='_');
}

inline bool IsNewline(char c)
{
	return c=='\n' || c=='\r';
}

inline char* SeekNewline(char *pC)
{
	while(!IsNewline(*pC) && *pC!=NULL) pC++;
	while(IsNewline(*pC)) pC++;
	return pC;
}

inline char* SkipWhite(char *pC)
{
	while(IsWhite(*pC)) pC++;
	return pC;
}

#endif
