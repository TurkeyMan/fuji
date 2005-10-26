#if !defined(_MFTYPES_H)
#define _MFTYPES_H

// if SSE optimisations are enabled, include SSE intrinsics header
#if defined(_FUJI_SSE)
	#include "xmmintrin.h"
#endif

// Data Type Definition
#if defined(_FUJI_SSE)
typedef __m128				uint128;
#else
typedef unsigned int		uint128[4];
#endif

#if !defined(_DC) // Bloody Kallistos defines all these

#if defined(__GNUC__)
typedef unsigned long long	uint64;
typedef long long 			int64;
#else
typedef unsigned __int64	uint64;
typedef __int64				int64;
#endif

typedef unsigned int		uint32;
typedef int					int32;
typedef unsigned short		uint16;
typedef short				int16;
typedef unsigned char		uint8;
typedef char				int8;

#endif

// rect structure
struct MFRect
{
	float x, y;
	float width, height;
};

#endif // _MFTYPES_H
