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

	inline bool operator==(const Vector3 &v) const
	{
		return x==v.x && y==v.y && z==v.z;
	}

	inline bool operator!=(const Vector3 &v) const
	{
		return x!=v.x || y!=v.y || z!=v.z;
	}

	inline float MagSquared() const
	{
		return x*x + y*y + z*z;
	}

	inline float Magnitude() const
	{
		return (float)sqrt(x*x + y*y + z*z);
	}

	inline float Distance(const Vector3 &v) const
	{
		return (v-*this).Magnitude();
	}

	inline Vector3& Normalise()
	{
		return *this /= this->Magnitude();
	}

	inline float Dot(const Vector3 &v) const
	{
		return x*v.x + y*v.y + z*v.z;
	}

	inline Vector3 Cross(const Vector3 &v) const
	{
		Vector3 t;
		t.x = y*v.z + z*v.y;
		t.y = z*v.x + x*v.z;
		t.z = x*v.y + y*v.x;
		return t;
	}

	inline Vector3& Cross(const Vector3 &v, const Vector3 &v2)
	{
		x = v.y*v2.z + v.z*v2.y;
		y = v.z*v2.x + v.x*v2.z;
		z = v.x*v2.y + v.y*v2.x;
		return *this;
	}

	inline float Cross2(const Vector3 &v) const
	{
		return x*v.y - y*v.x;
	}

	inline float GetAngle()
	{
		float d = acosf(this->Dot(up));
		return (this->Cross2(up) >= 0.0f) ? d : 2.0f*PI - d;
	}

	Vector3& ApplyMatrix(Matrix &mat);
};

inline Vector3 Vector(float x, float y, float z = 0.0f) { Vector3 t; t.x = x; t.y = y; t.z = z; return t; }

#endif