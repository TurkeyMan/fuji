#if !defined(_VECTOR4_H)
#define _VECTOR4_H

#pragma warning(disable:4201)

#include "Common.h"
#include "Vector3.h"

class Matrix;

class _ALIGN16 Vector4
{
public:
	union
	{
		struct
		{
			float x,y,z,w;
		};

		uint128 packed;
	};

	static const Vector4 zero;
	static const Vector4 one;
	static const Vector4 identity;

	inline Vector4 operator-()						{ Vector4 t; t.x=-x; t.y=-y;	t.z=-z;	t.w=-w; return t; }

	inline bool operator==(const Vector4 &v) const	{ return x==v.x && y==v.y && z==v.z && w==v.w; }
	inline bool operator!=(const Vector4 &v) const	{ return x!=v.x || y!=v.y || z!=v.z || w!=v.w; }

	inline Vector4& operator=(const Vector4 &v)		{ x=v.x;	y=v.y;	z=v.z;	w=v.w;	return *this; }
	inline Vector4& operator+=(const Vector4 &v)	{ x+=v.x;	y+=v.y;	z+=v.z;	w+=v.w;	return *this; }
	inline Vector4& operator-=(const Vector4 &v)	{ x-=v.x;	y-=v.y;	z-=v.z;	w-=v.w;	return *this; }
	inline Vector4& operator*=(float f)				{ x*=f;		y*=f;	z*=f;	w*=f;	return *this; }
	inline Vector4& operator*=(const Vector4 &v)	{ x*=v.x;	y*=v.y;	z*=v.z;	w*=v.w;	return *this; }
	inline Vector4& operator/=(float f)				{ x/=f;		y/=f;	z/=f;	w/=f;	return *this; }
	inline Vector4& operator/=(const Vector4 &v)	{ x/=v.x;	y/=v.y;	z/=v.z; w/=v.w;	return *this; }

	inline Vector4 operator+(const Vector4 &v) const	{ Vector4 t; t.x = x+v.x;	t.y = y+v.y;	t.z = z+v.z;	t.w = w+v.w;	return t; }
	inline Vector4 operator-(const Vector4 &v) const	{ Vector4 t; t.x = x-v.x;	t.y = y-v.y;	t.z = z-v.z;	t.w = w-v.w;	return t; }
	inline Vector4 operator*(float f) const				{ Vector4 t; t.x = x*f;		t.y = y*f;		t.z = z*f;		t.w = w*f;		return t; }
	inline Vector4 operator*(const Vector4 &v) const	{ Vector4 t; t.x = x*v.x;	t.y = y*v.y;	t.z = z*v.z;	t.w = w*v.w;	return t; }
	inline Vector4 operator/(float f) const				{ Vector4 t; t.x = x/f;		t.y = y/f;		t.z = z/f;		t.w = w/f;		return t; }
	inline Vector4 operator/(const Vector4 &v) const	{ Vector4 t; t.x = x/v.x;	t.y = y/v.y;	t.z = z/v.z;	t.w = w/v.w;	return t; }

	inline Vector3& ToVector3()			{ return *(Vector3*)this; }
	inline Vector3& ToVector3() const	{ return *(Vector3*)this; }

	inline operator float*()			{ return (float*)this; }
	inline operator float*() const		{ return (float*)this; }

	inline uint32 ToARGB() const			{ return ((uint32)(w*255.0f)<<24) | ((uint32)(x*255.0f)<<16) | ((uint32)(y*255.0f)<<8) | (uint32)(z*255.0f); }
	inline Vector4& FromARGB(uint32 col)	{ x = (float)((col&0xFF0000)>>16)/255.0f; y = (float)((col&0xFF00)>>8)/255.0f; z = (float)(col&0xFF)/255.0f; w = (float)((col&0xFF000000)>>24)/255.0f; return *this; }

	inline float Dot(const Vector4 &vec) const { return x*vec.x + y*vec.y + z*vec.z + w*vec.w; }
	inline float MagSquared() const { return x*x + y*y + z*z + w*w; }
	inline float Magnitude() const { return MFSqrt(x*x + y*y + z*z + w*w); }
	inline Vector4& Normalise() { float l = MFSqrt(x*x + y*y + z*z + w*w); *this /= l; return *this; }
	inline Vector4& Lerp(const Vector4 &v, float t) { *this += (v-*this)*t; return *this; }

	inline char* ToString() const { return STR("%.2f, %.2f, %.2f, %.2f", x, y, z, w); }

	Vector4& ApplyMatrix(const Matrix &mat);
	Vector4& ApplyMatrix3x3(const Matrix &mat);
};

inline Vector4 operator*(float f, Vector4 v) { v.x *= f; v.y *= f; v.z *= f; v.w *= f; return v; }

inline Vector4 Vector(float x, float y, float z, float w) { Vector4 t; t.x = x; t.y = y; t.z = z; t.w = w; return t; }
inline Vector4 Vector(const Vector3 &v, float w) { Vector4 t; t.x = v.x; t.y = v.y; t.z = v.z; t.w = w; return t; }

template <>
inline Vector4 Min(Vector4 a, Vector4 b) { Vector4 t; t.x = a.x < b.x ? a.x : b.x; t.y = a.y < b.y ? a.y : b.y; t.z = a.z < b.z ? a.z : b.z; t.w = a.w < b.w ? a.w : b.w; return t; }
template <>
inline Vector4 Max(Vector4 a, Vector4 b) { Vector4 t; t.x = a.x > b.x ? a.x : b.x; t.y = a.y > b.y ? a.y : b.y; t.z = a.z > b.z ? a.z : b.z; t.w = a.w < b.w ? a.w : b.w; return t; }

#endif
