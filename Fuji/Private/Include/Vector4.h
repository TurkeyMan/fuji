#if !defined(_VECTOR4_H)
#define _VECTOR4_H

#include <math.h>
#include "Common.h"
#include "Vector3.h"

class Matrix;

class Vector4
{
public:
	float x,y,z,w;

	static const Vector4 zero;
	static const Vector4 one;

	inline Vector4 operator-()						{ Vector4 t; t.x=-x; t.y=-y;	t.z=-z;	t.w=-w; return t; }

	inline bool operator==(const Vector4 &v) const { return x==v.x && y==v.y && z==v.z && w==v.w; }
	inline bool operator!=(const Vector4 &v) const { return x!=v.x || y!=v.y || z!=v.z || w!=v.w; }

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

	inline Vector3& ToVector3() { return *(Vector3*)this; }
	inline uint32 ToColour() { return ((uint32)(w*255.0f)<<24) | ((uint32)(x*255.0f)<<16) | ((uint32)(y*255.0f)<<8) | (uint32)(z*255.0f); }
	inline Vector4& FromColour(uint32 col) { x = (float)((col&0xFF0000)>>16)/255.0f; y = (float)((col&0xFF00)>>8)/255.0f; z = (float)(col&0xFF)/255.0f; w = (float)((col&0xFF000000)>>24)/255.0f; return *this; }

	inline float Dot(const Vector4 &vec) const { return x*vec.x + y*vec.y + z*vec.z + w*vec.w; }
	inline float MagSquared() const { return x*x + y*y + z*z + w*w; }
	inline float Magnitude() const { return sqrtf(x*x + y*y + z*z + w*w); }
	inline Vector4& Normalise() { float l = sqrtf(x*x + y*y + z*z + w*w); *this /= l; return *this; }
	inline Vector4& Lerp(const Vector4 &v, float t) { *this += (v-*this)*t; return *this; }

	inline char* ToString() const { return STR("%.2f, %.2f, %.2f, %.2f", x, y, z, w); }

	Vector4& ApplyMatrix(const Matrix &mat);
	Vector4& ApplyMatrix3x3(const Matrix &mat);
};

inline Vector4 Vector(float x, float y, float z, float w) { Vector4 t; t.x = x; t.y = y; t.z = z; t.w = w; return t; }
inline Vector4 Vector(Vector3 v, float w) { Vector4 t; t.x = v.x; t.y = v.y; t.z = v.z; t.w = w; return t; }

#endif
