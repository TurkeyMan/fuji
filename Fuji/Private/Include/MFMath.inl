#include <math.h>

inline float MFSin(float angle)
{
	return sinf(angle);
}

inline float MFCos(float angle)
{
	return cosf(angle);
}

inline float MFTan(float angle)
{
	return tanf(angle);
}

inline float MFASin(float angle)
{
	return asinf(angle);
}

inline float MFACos(float angle)
{
	return acosf(angle);
}

inline float MFATan(float angle)
{
	return atanf(angle);
}

inline float MFRSqrt(float x)
{
	return 1.0f / sqrtf(x);
}

inline float MFSqrt(float x)
{
	return sqrtf(x);
}

inline float MFRcp(float x)
{
	return 1.0f / x;
}

inline float MFPow(float x, float y)
{
	return powf(x, y);
}

template <typename T>
inline T MFAbs(T x)
{
	return (x < (T)0) ? -x : x;
}

template <class T>
inline T MFMin(T a, T b)
{
	return a < b ? a : b;
}

template <class T>
inline T MFMax(T a, T b)
{
	return a > b ? a : b;
}

template <class T>
inline T MFClamp(T x, T y, T z)
{
	return MFMax(x, MFMin(y, z));
}

inline float MFRand_Unit()
{
	return (float)MFRand()*(float)(1.0/4294967295.0); 
}

inline double MFRand_Double() 
{ 
	uint32 a=MFRand()>>5, b=MFRand()>>6; 
	return (a*67108864.0+b)*(1.0/9007199254740992.0); 
} 

inline float MFRand_Range(float min, float max)
{
	return min + MFRand_Unit()*(max-min);
}
