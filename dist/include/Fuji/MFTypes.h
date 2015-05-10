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
	#include <xmmintrin.h>
#endif

// Data Type Definition
#if defined(_FUJI_SSE)
typedef __m128				uint128;
#else
typedef unsigned int		uint128[4];
#endif

#if !defined(_DC) // Bloody Kallistos defines all these

#if defined(__GNUC__)
#include <stddef.h>
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
#if defined(_WIN32)
	#if defined(_WIN64)
		typedef __int64		intp;
	#else
		typedef _W64 long	intp;
	#endif
#elif defined(MF_COMPILER_CLANG)
	#if defined(MF_64BIT)
		typedef int64		intp;
	#else
		typedef int32		intp;
	#endif
#else
	typedef ssize_t			intp;
#endif

#endif

/**
 * Implements a D language 'slice' structure.
 */
template<typename T>
struct DSlice
{
	T *ptr;
	size_t length;

	// constructors
	DSlice<T>() : ptr(NULL), length(0) {}
	DSlice<T>(T* ptr, size_t length) : ptr(ptr), length(length) {}
	template <typename U>
	DSlice<T>(DSlice<U> rh) : ptr(rh.ptr), length(rh.length) {}

	// assignment
	template <typename U>
	DSlice<T>& operator =(DSlice<U> rh) { length = rh.length; ptr = rh.ptr; return *this; }

	// contents
	T& operator[](size_t i)
	{
		MFDebug_Assert(i < length, "Index out of range!");
		return ptr[i];
	}
	const T& operator[](size_t i) const
	{
		MFDebug_Assert(i < length, "Index out of range!");
		return ptr[i];
	}

	DSlice<T> slice(size_t first, size_t last) const
	{
		MFDebug_Assert(first <= last && last <= length, "Index out of range!");
		return DSlice<T>(ptr + first, last - first);
	}

	DSlice<T> popFront(size_t count = 1)
	{
		return DSlice(ptr + count, length - count);
	}
	DSlice<T> popBack(size_t count = 1)
	{
		return DSlice(ptr, length - count);
	}

	bool empty() const
	{
		return length == 0;
	}

	// comparison
	bool operator ==(DSlice<const T> rh) const
	{
		return ptr == rh.ptr && length == rh.length;
	}
	bool operator !=(DSlice<const T> rh) const
	{
		return ptr != rh.ptr || length != rh.length;
	}

	template <typename U>
	bool eq(DSlice<U> rh) const
	{
		if(length != rh.length)
			return false;
		for(size_t i=0; i<length; ++i)
			if(ptr[i] != rh.ptr[i])
				return false;
		return true;
	}
};

/**
 * Represents a spatial rectangle.
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
	return x >= pRect->x && x < pRect->x + pRect->width &&
			y >= pRect->y && y < pRect->y + pRect->height;
}

inline bool MFTypes_RectOverlap(MFRect *pRect1, MFRect *pRect2)
{
	return pRect1->x < pRect2->x + pRect2->width && pRect1->x + pRect1->width > pRect2->x &&
			pRect1->y < pRect2->y + pRect2->height && pRect1->y + pRect1->height > pRect2->y;
}

#endif // _MFTYPES_H

/** @} */
