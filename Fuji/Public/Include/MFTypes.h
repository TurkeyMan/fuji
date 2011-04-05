/**
 * @file MFTypes.h
 * @brief Standard types used by the Mount Fuji Engine.
 * @author Manu Evans
 * @defgroup MFTypes Standard Types
 * @{
 */

/** @typedef uint128
 * 128 bit unsigned integer value. */
/** @typedef uint64
 * 64 bit unsigned integer value. */
/** @typedef int64
 * 64 bit signed integer value. */
/** @typedef uint32
 * 32 bit unsigned integer value. */
/** @typedef int32
 * 32 bit signed integer value. */
/** @typedef uint16
 * 16 bit unsigned integer value. */
/** @typedef int16
 * 16 bit signed integer value. */
/** @typedef uint8
 * 8 bit unsigned integer value. */
/** @typedef int8
 * 8 bit signed integer value. */

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

// pointer sized ints
typedef size_t				uintp;
#if defined(WIN32)
	#if defined(_WIN64)
		typedef __int64		intp;
	#else
		typedef _W64 long	intp;
	#endif
#else
typedef ssize_t				intp;
#endif

#endif

/**
 * Represents a spacial rectangle.
 */
struct MFRect
{
	float x; /**< X coordinate of rectangle */
	float y; /**< Y coordinate of rectangle */
	float width; /**< Width of rectangle */
	float height; /**< Height of rectangle */
};

inline bool MFTypes_PointInRect(float x, float y, MFRect *pRect)
{
	return x >= pRect->x && x <= pRect->x + pRect->width &&
			y >= pRect->y && y <= pRect->y + pRect->height;
}

inline bool MFTypes_RectOverlap(MFRect *pRect1, MFRect *pRect2)
{
	return false;
}

#endif // _MFTYPES_H

/** @} */
