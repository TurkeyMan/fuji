#include "MFMatrix.h"

inline void MFVector::Set(float _x, float _y, float _z, float _w)
{
	x = _x;
	y = _y;
	z = _z;
	w = _w;
}

inline void MFVector::Swizzle(const MFVector &source, const uint8 _x, const uint8 _y, const uint8 _z, const uint8 _w)
{
	if(_x&SW_NEG)
		x = -source[_x&SW_ComponentMask];
	else
		x = source[_x&SW_ComponentMask];

	if(_y&SW_NEG)
		y = -source[_y&SW_ComponentMask];
	else
		y = source[_y&SW_ComponentMask];

	if(_z&SW_NEG)
		z = -source[_z&SW_ComponentMask];
	else
		z = source[_z&SW_ComponentMask];

	if(_w&SW_NEG)
		w = -source[_w&SW_ComponentMask];
	else
		w = source[_w&SW_ComponentMask];

	if(_x&SW_ABS) x = MFAbs(x);
	if(_y&SW_ABS) y = MFAbs(y);
	if(_z&SW_ABS) z = MFAbs(z);
	if(_w&SW_ABS) w = MFAbs(w);
}

inline bool MFVector::operator==(const MFVector &v) const
{
	return x==v.x && y==v.y && z==v.z && w==v.w;
}

inline bool MFVector::operator!=(const MFVector &v) const
{
	return x!=v.x || y!=v.y || z!=v.z || w!=v.w;
}

inline MFVector MFVector::operator-() const
{
	MFVector t;

	t.x=-x;
	t.y=-y;
	t.z=-z;
	t.w=-w;

	return t;
}

inline MFVector& MFVector::operator=(const MFVector &v)
{
	x=v.x;
	y=v.y;
	z=v.z;
	w=v.w;

	return *this;
}

inline MFVector& MFVector::operator+=(const MFVector &v)
{
	x+=v.x;
	y+=v.y;
	z+=v.z;
	w+=v.w;

	return *this;
}

inline MFVector& MFVector::operator-=(const MFVector &v)
{
	x-=v.x;
	y-=v.y;
	z-=v.z;
	w-=v.w;

	return *this;
}

inline MFVector& MFVector::operator*=(float f)
{
	x*=f;
	y*=f;
	z*=f;
	w*=f;

	return *this;
}

inline MFVector& MFVector::operator*=(const MFVector &v)
{
	x*=v.x;
	y*=v.y;
	z*=v.z;
	w*=v.w;

	return *this;
}

/*
inline MFVector& MFVector::operator/=(float f)
{
	x/=f;
	y/=f;
	z/=f;
	w/=f;

	return *this;
}

inline MFVector& MFVector::operator/=(const MFVector &v)
{
	x/=v.x;
	y/=v.y;
	z/=v.z;
	w/=v.w;

	return *this;
}
*/

inline MFVector MFVector::operator+(const MFVector &v) const
{
	MFVector t;

	t.x = x+v.x;
	t.y = y+v.y;
	t.z = z+v.z;
	t.w = w+v.w;

	return t;
}

inline MFVector MFVector::operator-(const MFVector &v) const
{
	MFVector t;

	t.x = x-v.x;
	t.y = y-v.y;
	t.z = z-v.z;
	t.w = w-v.w;

	return t;
}

inline MFVector MFVector::operator*(float f) const
{
	MFVector t;

	t.x = x*f;
	t.y = y*f;
	t.z = z*f;
	t.w = w*f;

	return t;
}

inline MFVector MFVector::operator*(const MFVector &v) const
{
	MFVector t;

	t.x = x*v.x;
	t.y = y*v.y;
	t.z = z*v.z;
	t.w = w*v.w;

	return t;
}

/*
inline MFVector MFVector::operator/(float f) const
{
	MFVector t;

	f = 1.0f/f;

	t.x = x*f;
	t.y = y*f;
	t.z = z*f;
	t.w = w*f;

	return t;
}

inline MFVector MFVector::operator/(const MFVector &v) const
{
	MFVector t;

	t.x = x/v.x;
	t.y = y/v.y;
	t.z = z/v.z;
	t.w = w/v.w;

	return t;
}
*/

inline MFVector& MFVector::Add4(const MFVector &v1, const MFVector &v2)
{
	x = v1.x + v2.x;
	y = v1.y + v2.y;
	z = v1.z + v2.z;
	w = v1.w + v2.w;
	return *this;
}

inline MFVector& MFVector::Sub4(const MFVector &v1, const MFVector &v2)
{
	x = v1.x - v2.x;
	y = v1.y - v2.y;
	z = v1.z - v2.z;
	w = v1.w - v2.w;
	return *this;
}

inline MFVector& MFVector::Mul4(const MFVector &v1, float f)
{
	x = v1.x * f;
	y = v1.y * f;
	z = v1.z * f;
	w = v1.w * f;
	return *this;
}

inline MFVector& MFVector::Mul4(const MFVector &v1, const MFVector &v2)
{
	x = v1.x * v2.x;
	y = v1.y * v2.y;
	z = v1.z * v2.z;
	w = v1.w * v2.w;
	return *this;
}

inline MFVector& MFVector::Mad4(const MFVector &v1, float f, const MFVector &v3)
{
	x = v1.x * f + v3.x;
	y = v1.y * f + v3.y;
	z = v1.z * f + v3.z;
	w = v1.w * f + v3.w;
	return *this;
}

inline MFVector& MFVector::Mad4(const MFVector &v1, const MFVector &v2, const MFVector &v3)
{
	x = v1.x * v2.x + v3.x;
	y = v1.y * v2.y + v3.y;
	z = v1.z * v2.z + v3.z;
	w = v1.w * v2.w + v3.w;
	return *this;
}

/*
inline MFVector& MFVector::Div4(const MFVector &v1, float f)
{
	f = 1.0f / f;
	x = v1.x * f;
	y = v1.y * f;
	z = v1.z * f;
	w = v1.w * f;
	return *this;
}

inline MFVector& MFVector::Div4(const MFVector &v1, const MFVector &v2)
{
	x = v1.x / v2.x;
	y = v1.y / v2.y;
	z = v1.z / v2.z;
	w = v1.w / v2.w;
	return *this;
}
*/

inline MFVector& MFVector::Add3(const MFVector &v1, const MFVector &v2)
{
	x = v1.x + v2.x;
	y = v1.y + v2.y;
	z = v1.z + v2.z;
	return *this;
}

inline MFVector& MFVector::Sub3(const MFVector &v1, const MFVector &v2)
{
	x = v1.x - v2.x;
	y = v1.y - v2.y;
	z = v1.z - v2.z;
	return *this;
}

inline MFVector& MFVector::Mul3(const MFVector &v1, float f)
{
	x = v1.x * f;
	y = v1.y * f;
	z = v1.z * f;
	return *this;
}

inline MFVector& MFVector::Mul3(const MFVector &v1, const MFVector &v2)
{
	x = v1.x * v2.x;
	y = v1.y * v2.y;
	z = v1.z * v2.z;
	return *this;
}

inline MFVector& MFVector::Mad3(const MFVector &v1, float f, const MFVector &v3)
{
	x = v1.x * f + v3.x;
	y = v1.y * f + v3.y;
	z = v1.z * f + v3.z;
	return *this;
}

inline MFVector& MFVector::Mad3(const MFVector &v1, const MFVector &v2, const MFVector &v3)
{
	x = v1.x * v2.x + v3.x;
	y = v1.y * v2.y + v3.y;
	z = v1.z * v2.z + v3.z;
	return *this;
}

/*
inline MFVector& MFVector::Div3(const MFVector &v1, float f)
{
	f = 1.0f / f;
	x = v1.x * f;
	y = v1.y * f;
	z = v1.z * f;
	return *this;
}

inline MFVector& MFVector::Div3(const MFVector &v1, const MFVector &v2)
{
	x = v1.x / v2.x;
	y = v1.y / v2.y;
	z = v1.z / v2.z;
	return *this;
}
*/

inline MFVector& MFVector::Add2(const MFVector &v1, const MFVector &v2)
{
	x = v1.x + v2.x;
	y = v1.y + v2.y;
	return *this;
}

inline MFVector& MFVector::Sub2(const MFVector &v1, const MFVector &v2)
{
	x = v1.x - v2.x;
	y = v1.y - v2.y;
	return *this;
}

inline MFVector& MFVector::Mul2(const MFVector &v1, float f)
{
	x = v1.x * f;
	y = v1.y * f;
	return *this;
}

inline MFVector& MFVector::Mul2(const MFVector &v1, const MFVector &v2)
{
	x = v1.x * v2.x;
	y = v1.y * v2.y;
	return *this;
}

inline MFVector& MFVector::Mad2(const MFVector &v1, float f, const MFVector &v3)
{
	x = v1.x * f + v3.x;
	y = v1.y * f + v3.y;
	return *this;
}

inline MFVector& MFVector::Mad2(const MFVector &v1, const MFVector &v2, const MFVector &v3)
{
	x = v1.x * v2.x + v3.x;
	y = v1.y * v2.y + v3.y;
	return *this;
}

/*
inline MFVector& MFVector::Div2(const MFVector &v1, float f)
{
	f = 1.0f / f;
	x = v1.x * f;
	y = v1.y * f;
	return *this;
}

inline MFVector& MFVector::Div2(const MFVector &v1, const MFVector &v2)
{
	x = v1.x / v2.x;
	y = v1.y / v2.y;
	return *this;
}
*/

inline MFVector::operator float*()
{
	return (float*)this;
}

inline MFVector::operator const float*() const
{
	return (const float*)this;
}


// 'BGRA' in order of bytes
inline uint32 MFVector::ToPackedColour() const
{
#if defined(_WINPC) || defined(_MFXBOX)
	return ((uint32)(w*255.0f)<<24) |
		   ((uint32)(x*255.0f)<<16) |
		   ((uint32)(y*255.0f)<<8) |
		    (uint32)(z*255.0f);
#elif defined(_PSP)
	return ((uint32)(w*255.0f)<<24) |
		   ((uint32)(z*255.0f)<<16) |
		   ((uint32)(y*255.0f)<<8) |
		    (uint32)(x*255.0f);
#else
	return ((uint32)(w*255.0f)<<24) |
		   ((uint32)(x*255.0f)<<16) |
		   ((uint32)(y*255.0f)<<8) |
		    (uint32)(z*255.0f);
#endif
}

inline MFVector& MFVector::FromPackedColour(uint32 col)
{
#if defined(_WINPC) || defined(_MFXBOX)
	z = (float) (col&0xFF) * (1.0f/255.0f);
	y = (float)((col&0xFF00)>>8) * (1.0f/255.0f);
	x = (float)((col&0xFF0000)>>16) * (1.0f/255.0f);
	w = (float)((col&0xFF000000)>>24) * (1.0f/255.0f);
#elif defined(_PSP)
	x = (float) (col&0xFF) * (1.0f/255.0f);
	y = (float)((col&0xFF00)>>8) * (1.0f/255.0f);
	z = (float)((col&0xFF0000)>>16) * (1.0f/255.0f);
	w = (float)((col&0xFF000000)>>24) * (1.0f/255.0f);
#else
	z = (float) (col&0xFF) * (1.0f/255.0f);
	y = (float)((col&0xFF00)>>8) * (1.0f/255.0f);
	x = (float)((col&0xFF0000)>>16) * (1.0f/255.0f);
	w = (float)((col&0xFF000000)>>24) * (1.0f/255.0f);
#endif

	return *this;
}

inline MFVector& MFVector::Rcp4(const MFVector &v)
{
	x = MFRcp(v.x);
	y = MFRcp(v.y);
	z = MFRcp(v.z);
	w = MFRcp(v.w);

	return *this;
}

inline MFVector& MFVector::Rcp3(const MFVector &v)
{
	x = MFRcp(v.x);
	y = MFRcp(v.y);
	z = MFRcp(v.z);

	return *this;
}

inline MFVector& MFVector::Rcp2(const MFVector &v)
{
	x = MFRcp(v.x);
	y = MFRcp(v.y);

	return *this;
}

inline float MFVector::Dot4(const MFVector &vec) const
{
	return x*vec.x + y*vec.y + z*vec.z + w*vec.w;
}

inline float MFVector::DotH(const MFVector &vec) const
{
	return x*vec.x + y*vec.y + z*vec.z + vec.w;
}

inline float MFVector::Dot3(const MFVector &vec) const
{
	return x*vec.x + y*vec.y + z*vec.z;
}

inline float MFVector::Dot2(const MFVector &vec) const
{
	return x*vec.x + y*vec.y;
}

//inline MFVector MFVector::Cross4(const MFVector &vec) const;

inline MFVector MFVector::Cross3(const MFVector &v) const
{
	MFVector t;

	t.x = y*v.z - z*v.y;
	t.y = z*v.x - x*v.z;
	t.z = x*v.y - y*v.x;

	return t;
}


inline float MFVector::Cross2(const MFVector &vec) const
{
	return 0.0f;
}

//inline MFVector& MFVector::Cross4(const MFVector &vec, const MFVector &vec2);

inline MFVector& MFVector::Cross3(const MFVector &v, const MFVector &v2)
{
	x = v.y*v2.z - v.z*v2.y;
	y = v.z*v2.x - v.x*v2.z;
	z = v.x*v2.y - v.y*v2.x;

	return *this;
}

inline float MFVector::MagSquared4() const
{
	return x*x + y*y + z*z + w*w;
}

inline float MFVector::MagSquared3() const
{
	return x*x + y*y + z*z;
}

inline float MFVector::MagSquared2() const
{
	return x*x + y*y;
}

inline float MFVector::Magnitude4() const
{
	return MFSqrt(x*x + y*y + z*z + w*w);
}

inline float MFVector::Magnitude3() const
{
	return MFSqrt(x*x + y*y + z*z);
}

inline float MFVector::Magnitude2() const
{
	return MFSqrt(x*x + y*y);
}

inline float MFVector::InvMagnitude4() const
{
	return MFRSqrt(x*x + y*y + z*z + w*w);
}

inline float MFVector::InvMagnitude3() const
{
	return MFRSqrt(x*x + y*y + z*z);
}

inline float MFVector::InvMagnitude2() const
{
	return MFRSqrt(x*x + y*y);
}

inline MFVector& MFVector::Normalise4(const MFVector &vec)
{
	Mul4(vec, vec.InvMagnitude4());
	return *this;
}

inline MFVector& MFVector::Normalise3(const MFVector &vec)
{
	Mul3(vec, vec.InvMagnitude3());
	return *this;
}

inline MFVector& MFVector::Normalise2(const MFVector &vec)
{
	Mul2(vec, vec.InvMagnitude2());
	return *this;
}

inline MFVector& MFVector::Normalise4()
{
	Mul4(*this, InvMagnitude4());
	return *this;
}

inline MFVector& MFVector::Normalise3()
{
	Mul3(*this, InvMagnitude3());
	return *this;
}

inline MFVector& MFVector::Normalise2()
{
	Mul2(*this, InvMagnitude2());
	return *this;
}

inline float MFVector::Distance(const MFVector &v) const
{
	return (v-*this).Magnitude3();
}

inline float MFVector::GetAngle(const MFVector &ref) const
{
	float d = MFACos(Dot3(ref));
	return (Cross2(ref) >= 0.0f) ? d : 2.0f*MFPI - d;
}


inline MFVector& MFVector::Lerp(const MFVector &v, float t)
{
	*this += (v-*this)*t;
	return *this;
}


inline const char * MFVector::ToString4() const
{
	return MFStr("%.2f, %.2f, %.2f, %.2f", x, y, z, w);
}

inline const char * MFVector::ToString3() const
{
	return MFStr("%.2f, %.2f, %.2f", x, y, z);
}

inline const char * MFVector::ToString2() const
{
	return MFStr("%.2f, %.2f", x, y);
}

inline MFVector operator*(float f, const MFVector &v)
{
	MFVector t;

	t.x = v.x*f;
	t.y = v.y*f;
	t.z = v.z*f;
	t.w = v.w*f;

	return t;
}


inline MFVector MakeVector(float f)
{
	MFVector t;

	t.x = f;
	t.y = f;
	t.z = f;
	t.w = f;

	return t;
}

inline MFVector MakeVector(float x, float y, float z, float w)
{
	MFVector t;

	t.x = x;
	t.y = y;
	t.z = z;
	t.w = w;

	return t;
}

inline MFVector MakeVector(const MFVector &v, float w)
{
	MFVector t;
	t.x = v.x;
	t.y = v.y;
	t.z = v.z;
	t.w = w;
	return t;
}


inline MFVector MFAbs(const MFVector &v)
{
	MFVector t;

	t.x = MFAbs(v.x);
	t.y = MFAbs(v.y);
	t.z = MFAbs(v.z);
	t.w = MFAbs(v.w);

	return t;
}

inline MFVector MFMin(const MFVector &a, const MFVector &b)
{
	MFVector t;

	t.x = a.x < b.x ? a.x : b.x;
	t.y = a.y < b.y ? a.y : b.y;
	t.z = a.z < b.z ? a.z : b.z;
	t.w = a.w < b.w ? a.w : b.w;

	return t;
}

inline MFVector MFMax(const MFVector &a, const MFVector &b)
{
	MFVector t;

	t.x = a.x > b.x ? a.x : b.x;
	t.y = a.y > b.y ? a.y : b.y;
	t.z = a.z > b.z ? a.z : b.z;
	t.w = a.w > b.w ? a.w : b.w;

	return t;
}

inline MFVector MFRand_Vector()
{
	return MakeVector(MFRand_Range(-1.0f, 1.0f), MFRand_Range(-1.0f, 1.0f), MFRand_Range(-1.0f, 1.0f), MFRand_Range(-1.0f, 1.0f));
}
