#if !defined(_VECTOR3_H)
#define _VECTOR3_H

#include <math.h>
#include "Common.h"

class Matrix;
class Vector4;

class Vector3
{
public:
	float x,y,z,anon;

	static const Vector3 zero;
	static const Vector3 one;
	static const Vector3 up;

	inline Vector3 operator-()						{ Vector3 t; t.x=-x; t.y=-y; t.z=-z; return t; }

	inline bool operator==(const Vector3 &v) const { return x==v.x && y==v.y && z==v.z; }
	inline bool operator!=(const Vector3 &v) const { return x!=v.x || y!=v.y || z!=v.z; }

	inline Vector3& operator=(const Vector3 &v)		{ x=v.x;	y=v.y;	z=v.z;	return *this; }
	inline Vector3& operator+=(const Vector3 &v)	{ x+=v.x;	y+=v.y;	z+=v.z;	return *this; }
	inline Vector3& operator-=(const Vector3 &v)	{ x-=v.x;	y-=v.y;	z-=v.z;	return *this; }
	inline Vector3& operator*=(float f)				{ x*=f;		y*=f;	z*=f;	return *this; }
	inline Vector3& operator*=(const Vector3 &v)	{ x*=v.x;	y*=v.y;	z*=v.z;	return *this; }
	inline Vector3& operator/=(float f)				{ x/=f;		y/=f;	z/=f;	return *this; }
	inline Vector3& operator/=(const Vector3 &v)	{ x/=v.x;	y/=v.y;	z/=v.z; return *this; }

	inline Vector3 operator+(const Vector3 &v) const	{ Vector3 t; t.x = x+v.x;	t.y = y+v.y;	t.z = z+v.z;	return t; }
	inline Vector3 operator-(const Vector3 &v) const	{ Vector3 t; t.x = x-v.x;	t.y = y-v.y;	t.z = z-v.z;	return t; }
	inline Vector3 operator*(float f) const				{ Vector3 t; t.x = x*f;		t.y = y*f;		t.z = z*f;		return t; }
	inline Vector3 operator*(const Vector3 &v) const	{ Vector3 t; t.x = x*v.x;	t.y = y*v.y;	t.z = z*v.z;	return t; }
	inline Vector3 operator/(float f) const				{ Vector3 t; t.x = x/f;		t.y = y/f;		t.z = z/f;		return t; }
	inline Vector3 operator/(const Vector3 &v) const	{ Vector3 t; t.x = x/v.x;	t.y = y/v.y;	t.z = z/v.z;	return t; }

	inline Vector4& ToVector4() { return *(Vector4*)this; }

	inline float Dot(const Vector3 &vec) const { return x*vec.x + y*vec.y + z*vec.z; }
	inline float MagSquared() const { return x*x + y*y + z*z; }
	inline float Magnitude() const { return sqrtf(x*x + y*y + z*z); }
	inline float Normalise() { float l = sqrtf(x*x + y*y + z*z); *this /= l; return l; }
	inline Vector3 Lerp(const Vector3 &v, float t) const { return *this + (v-*this)*t; }
	inline Vector3 Cross(const Vector3 &v) const { Vector3 t; t.x=y*v.z-v.y*z; t.y=z*v.x-v.z*x; t.z=x*v.y-v.x*y; return t; }
	inline Vector3& Cross(const Vector3 &v, const Vector3 &v2) { x = v.y*v2.z + v.z*v2.y; y = v.z*v2.x + v.x*v2.z; z = v.x*v2.y + v.y*v2.x; return *this; }
	inline float Cross2(const Vector3 &v) const { return x*v.y - y*v.x; }
	inline float Distance(const Vector3 &v) const { return (v-*this).Magnitude(); }
	inline float GetAngle() { float d = acosf(this->Dot(up)); return (this->Cross2(up) >= 0.0f) ? d : 2.0f*PI - d; }

	inline char* ToString() const { return STR("%.2f, %.2f, %.2f", x, y, z); }

	Vector3& ApplyMatrix(const Matrix &mat);
	Vector3& ApplyMatrix3x3(const Matrix &mat);
};

inline Vector3 Vector(float x, float y, float z = 0.0f) { Vector3 t; t.x = x; t.y = y; t.z = z; return t; }

#endif