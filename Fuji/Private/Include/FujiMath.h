#if !defined(_FUJI_MATH)
#define _FUJI_MATH

#include <math.h>

inline float MFSin(float angle)
{
#if !defined(_DC)
	return sinf(angle);
#else
	return (float)sin((float)angle);
#endif
}

inline float MFCos(float angle)
{
#if !defined(_DC)
	return cosf(angle);
#else
	return (float)cos((float)angle);
#endif
}

inline float MFTan(float angle)
{
#if !defined(_DC)
	return tanf(angle);
#else
	return (float)tan((float)angle);
#endif
}

inline float MFASin(float angle)
{
#if !defined(_DC)
	return asinf(angle);
#else
	return (float)asin((float)angle);
#endif
}

inline float MFACos(float angle)
{
#if !defined(_DC)
	return acosf(angle);
#else
	return (float)acos((float)angle);
#endif
}

inline float MFATan(float angle)
{
#if !defined(_DC)
	return atanf(angle);
#else
	return (float)atan((float)angle);
#endif
}

inline float MFSqrt(float x)
{
#if !defined(_DC)
	return sqrtf(x);
#else
	return (float)sqrt((float)x);
#endif
}

template <typename T>
inline T MFAbs(T x)
{
	return (x < (T)0) ? -x : x;
}

#endif
