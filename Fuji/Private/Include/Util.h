#if !defined(_UTIL_H)
#define _UTIL_H

//#define HARD_ASSERTS

// this will print the assert message and the current
// callstack on the screen and enter an infinite draw loop 
#if defined(_XBOX) && !defined(HARD_ASSERTS)
	#define HARD_ASSERTS
#endif

class Vector3;

// triggers a debug BREAKPOINT
#if defined(_WINDOWS) || defined(_XBOX)
	#define BREAKPOINT { __asm { int 3 }; }
#elif defined(_LINUX)
	#define BREAKPOINT { asm("int $3"); }
#else
	#define BREAKPOINT

	// on systems without a debug break feature we want to throw a hard
	// assert so we can see the callstack and error
	#if !defined(HARD_ASSERTS)
		#define HARD_ASSERTS
	#endif
#endif

// asserts that a condition is true, if it fails, it throws an error and breaks the debugger..
#if !defined(_RETAIL)
	#if defined(HARD_ASSERTS)
		#define DBGASSERT(condition,str) { static bool ignore=false; if(!(condition) && !ignore) { hardAssert(#condition, str, __FILE__, __LINE__); BREAKPOINT; ignore=true; } }
	#else
		#define DBGASSERT(condition,str) { static bool ignore=false; if(!(condition) && !ignore) { dbgAssert(#condition, str, __FILE__, __LINE__); BREAKPOINT; ignore=true; } }
	#endif
#else
	#define DBGASSERT(x, y)
#endif

// assert a pointer is 16 byte alligned
#define ASSERT_ALLIGN16(x) DBGASSERT(!((uint32)((void*)(x))&0xF), STR("Pointer not 16 byte alligned: 0x%X", (void*)(x)))

void dbgAssert(const char *pReason, const char *pMessage, const char *pFile, int line);
void hardAssert(const char *pReason, const char *pMessage, const char *pFile, int line);
int dprintf(const char *format, ...);
void LOGD(const char *string);
char* STR(const char *format, ...);
char* STRn(const char *source, int n);

// string utilities
bool StrICompare(const char *s1, const char *s2);

// random number functions
uint32 Rand();
float RandomUnit();
float RandomRange(float min, float max);
Vector3 RandomVector();

// endian flipping
#if defined(BIG_ENDIAN)
#define FlipEndian(x) FLIP_ENDIAN(x)
#else
#define FlipEndian(x)
#endif

template <typename T>
inline void FLIP_ENDIAN(T *pData)
{
	register char t[sizeof(T)];

	for(int a=0; a<sizeof(T); a++)
	{
		t[a%4] = pData[(sizeof(T)-1-a)%4];
	}

	*pData = *(T*)t;
}

// some useful string parsing functions
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
	while(!IsNewline(*pC) && *pC != 0) pC++;
	while(IsNewline(*pC)) pC++;
	return pC;
}

inline char* SkipWhite(char *pC)
{
	while(IsWhite(*pC)) pC++;
	return pC;
}

#endif
