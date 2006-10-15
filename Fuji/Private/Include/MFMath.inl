#include <math.h>

//#define _USE_MFSQRT

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

#if defined(_USE_MFSQRT)
inline float MFFRExp(float x, int *pw2)
{
	union float_long { float f; long l; } fl;
    long int i;

    fl.f=x;
    // Find the exponent (power of 2)
    i  = (fl.l >> 23) & 0x000000ff;
    i -= 0x7e;
    *pw2 = i;
    fl.l &= 0x807fffff; // strip all exponent bits
    fl.l |= 0x3f000000; // mantissa between 0.5 and 1

    return fl.f;
}

inline float MFLDExp(float x, int pw2)
{
    union float_long { float f; long l; } fl;
    long e;

    fl.f = x;

    e=(fl.l >> 23) & 0x000000ff;
    e+=pw2;
    fl.l= ((e & 0xff) << 23) | (fl.l & 0x807fffff);

    return fl.f;
}
#endif

inline float MFSqrt(float x)
{
#if defined(_USE_MFSQRT)
	float f, y;
    int n;

    if(x==0.0f) return x;
    else if(x==1.0f) return 1.0f;
    else if(x<0.0f)
        return 0.0f;
    f=MFFRExp(x, &n);
    y=0.41731f+0.59016f*f; // Educated guess
    // For a 24 bit mantisa (float), two iterations are sufficient
    y+=f/y;
    y=MFLDExp(y, -2) + f/y; // Faster version of 0.25 * y + f/y

    if(n&1)
    {
        y*=0.7071067812f;
        ++n;
    }
    return MFLDExp(y, n/2);
#else
	return sqrtf(x);
#endif
}

inline float MFRSqrt(float x)
{
	// TODO: this should probably be considered
//	float s = sqrtf(x);
//	return s > 0.0f ? 1.0f / s : 0.0f;

	return 1.0f / MFSqrt(x);
}

inline float MFRSqrtE(float x)
{
	long i;
	float y, r;

	y = x * 0.5f;
	i = *(long *)&x;
	i = 0x5f3759df - (i>>1);
	r = *(float *)&i;
	r = r * (1.5f - r * r * y);

	return r;
}

inline float MFRcp(float x)
{
	// TODO: this should probably be considered
//	return x != 0.0f ? 1.0f / x : 0.0f;

	return 1.0f / x;
}

inline float MFPow(float x, float y)
{
	return powf(x, y);
}

inline float MFCeil(float x)
{
	return ceilf(x);
}

inline float MFFloor(float x)
{
	return floorf(x);
}

template <typename T>
inline T MFAbs(T x)
{
	return (x < (T)0) ? -x : x;
}

template <typename T>
inline T MFMin(T a, T b)
{
	return a < b ? a : b;
}

template <typename T>
inline T MFMax(T a, T b)
{
	return a > b ? a : b;
}

template <typename T>
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
