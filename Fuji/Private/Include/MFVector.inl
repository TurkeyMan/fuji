#include <Matrix.h>

inline MFVector MFVector::operator-()
{
	MFVector t;

	t.x=-x;
	t.y=-y;
	t.z=-z;
	t.w=-w;

	return t;
}

inline bool MFVector::operator==(const MFVector &v) const
{
	return x==v.x && y==v.y && z==v.z && w==v.w;
}

inline bool MFVector::operator!=(const MFVector &v) const
{
	return x!=v.x || y!=v.y || z!=v.z || w!=v.w;
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

	t.x = x/f;
	t.y = y/f;
	t.z = z/f;
	t.w = w/f;

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

inline MFVector::operator float*()
{
	return (float*)this;
}

inline MFVector::operator float*() const
{
	return (float*)this;
}


// 'BGRA' in order of bytes
inline uint32 MFVector::ToPackedColour() const
{
#if defined(_WINPC) || defined(_XBOX)
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
#if defined(_WINPC) || defined(_XBOX)
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

inline float MFVector::Dot4(const MFVector &vec) const
{
	return x*vec.x + y*vec.y + z*vec.z + w*vec.w;
}

inline float MFVector::DotH(const MFVector &vec4) const
{
	return x*vec4.x + y*vec4.y + z*vec4.z + vec4.w;
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

inline MFVector MFVector::Cross(const MFVector &v) const
{
	MFVector t;

	t.x = y*v.z - v.y*z;
	t.y = z*v.x - v.z*x;
	t.z = x*v.y - v.x*y;

	return t;
}


inline float MFVector::Cross2(const MFVector &vec) const
{
	return 0.0f;
}

//inline MFVector& MFVector::Cross4(const MFVector &vec, const MFVector &vec2);

inline MFVector& MFVector::Cross(const MFVector &v, const MFVector &v2)
{
	x = v.y*v2.z - v.z*v2.y;
	y = v.z*v2.x - v.x*v2.z;
	z = v.x*v2.y - v.y*v2.x;

	return *this;
}

inline MFVector& MFVector::ApplyMatrix(const Matrix &mat)
{
	float _x=x, _y=y, _z=z, _w=w;

	x = _x*mat.m[0][0] + _y*mat.m[1][0] + _z*mat.m[2][0] + _w*mat.m[3][0];
	y = _x*mat.m[0][1] + _y*mat.m[1][1] + _z*mat.m[2][1] + _w*mat.m[3][1];
	z = _x*mat.m[0][2] + _y*mat.m[1][2] + _z*mat.m[2][2] + _w*mat.m[3][2];
	w = _x*mat.m[0][3] + _y*mat.m[1][3] + _z*mat.m[2][3] + _w*mat.m[3][3];

	return *this;
}

inline MFVector& MFVector::ApplyMatrixH(const Matrix &mat)
{
	float _x=x, _y=y, _z=z;

	x = _x*mat.m[0][0] + _y*mat.m[1][0] + _z*mat.m[2][0] + mat.m[3][0];
	y = _x*mat.m[0][1] + _y*mat.m[1][1] + _z*mat.m[2][1] + mat.m[3][1];
	z = _x*mat.m[0][2] + _y*mat.m[1][2] + _z*mat.m[2][2] + mat.m[3][2];

	return *this;
}

inline MFVector& MFVector::ApplyMatrix3x3(const Matrix &mat)
{
	float _x=x, _y=y, _z=z;

	x = _x*mat.m[0][0] + _y*mat.m[1][0] + _z*mat.m[2][0];
	y = _x*mat.m[0][1] + _y*mat.m[1][1] + _z*mat.m[2][1];
	z = _x*mat.m[0][2] + _y*mat.m[1][2] + _z*mat.m[2][2];

	return *this;
}

inline float MFVector::MagSquared4() const
{
	return x*x + y*y + z*z + w*w;
}

inline float MFVector::MagSquared() const
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

inline float MFVector::Magnitude() const
{
	return MFSqrt(x*x + y*y + z*z);
}

inline float MFVector::Magnitude2() const
{
	return MFSqrt(x*x + y*y);
}

inline MFVector& MFVector::Normalise4()
{
	float l = MFSqrt(x*x + y*y + z*z + w*w);
	*this *= 1.0f/l;
	return *this;
}

inline MFVector& MFVector::Normalise()
{
	float l = MFSqrt(x*x + y*y + z*z);
	*this *= 1.0f/l;
	return *this;
}

inline MFVector& MFVector::Normalise2()
{
	float l = MFSqrt(x*x + y*y);
	*this *= 1.0f/l;
	return *this;
}

inline float MFVector::Distance(const MFVector &v) const
{
	return (v-*this).Magnitude();
}

inline float MFVector::GetAngle(const MFVector &ref)
{
	float d = MFACos(Dot3(ref));
	return (Cross2(ref) >= 0.0f) ? d : 2.0f*PI - d;
}


inline MFVector& MFVector::Lerp(const MFVector &v, float t)
{
	*this += (v-*this)*t;
	return *this;
}


inline char* MFVector::ToString4() const
{
	return STR("%.2f, %.2f, %.2f, %.2f", x, y, z, w);
}

inline char* MFVector::ToString3() const
{
	return STR("%.2f, %.2f, %.2f", x, y, z);
}

inline char* MFVector::ToString2() const
{
	return STR("%.2f, %.2f", x, y);
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

inline MFVector MakeVector(const Vector3 &v, float w)
{
	MFVector t;

	t.x = v.x;
	t.y = v.y;
	t.z = v.z;
	t.w = w;

	return t;
}


inline MFVector Min(const MFVector &a, const MFVector &b)
{
	MFVector t;

	t.x = a.x < b.x ? a.x : b.x;
	t.y = a.y < b.y ? a.y : b.y;
	t.z = a.z < b.z ? a.z : b.z;
	t.w = a.w < b.w ? a.w : b.w;

	return t;
}

inline MFVector Max(const MFVector &a, const MFVector &b)
{
	MFVector t;

	t.x = a.x > b.x ? a.x : b.x;
	t.y = a.y > b.y ? a.y : b.y;
	t.z = a.z > b.z ? a.z : b.z;
	t.w = a.w > b.w ? a.w : b.w;

	return t;
}
