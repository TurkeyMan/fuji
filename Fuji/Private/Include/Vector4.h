#if !defined(_VECTOR4_H)
#define _VECTOR4_H

#include <math.h>
#include "Common.h"

class Matrix;
class Vector3;

class Vector4
{
public:
	float x,y,z,w;

	static const Vector4 zero;
	static const Vector4 one;

	inline Vector4 operator-()						{ Vector4 t; t.x=-x; t.y=-y;	t.z=-z;	t.w=-w; return t; }

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

	inline bool operator==(const Vector4 &v) const
	{
		return x==v.x && y==v.y && z==v.z && w==v.w;
	}

	inline bool operator!=(const Vector4 &v) const
	{
		return x!=v.x || y!=v.y || z!=v.z || w!=v.w;
	}

	inline float MagSquared() const
	{
		return x*x + y*y + z*z + w*w;
	}

	inline float Magnitude() const
	{
		return (float)sqrt(x*x + y*y + z*z + w*w);
	}

	inline Vector4& Normalise()
	{
		return *this /= this->Magnitude();
	}

	inline float Dot(const Vector4 &v) const
	{
		return x*v.x + y*v.y + z*v.z + w*v.w;
	}

	Vector4& ApplyMatrix(Matrix &mat);
};

inline Vector4 Vector(float x, float y, float z, float w) { Vector4 t; t.x = x; t.y = y; t.z = z; t.w = w; return t; }

#endif
