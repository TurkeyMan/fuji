#if !defined(_VECTOR3_H)
#define _VECTOR3_H

#pragma warning(disable:4201)

class Matrix;
class Vector4;

class _ALIGN16 Vector3
{
public:
	union
	{
		struct
		{
			float x,y,z,anon;
		};

		uint128 packed;
	};

	static const Vector3 zero;
	static const Vector3 one;
	static const Vector3 up;

	inline Vector3 operator-() const					{ Vector3 t; t.x=-x; t.y=-y; t.z=-z; return t; }

	inline bool operator==(const Vector3 &v) const		{ return x==v.x && y==v.y && z==v.z; }
	inline bool operator!=(const Vector3 &v) const		{ return x!=v.x || y!=v.y || z!=v.z; }

	inline Vector3& operator=(const Vector3 &v)			{ x=v.x;	y=v.y;	z=v.z;	return *this; }
	inline Vector3& operator+=(const Vector3 &v)		{ x+=v.x;	y+=v.y;	z+=v.z;	return *this; }
	inline Vector3& operator-=(const Vector3 &v)		{ x-=v.x;	y-=v.y;	z-=v.z;	return *this; }
	inline Vector3& operator*=(float f)					{ x*=f;		y*=f;	z*=f;	return *this; }
	inline Vector3& operator*=(const Vector3 &v)		{ x*=v.x;	y*=v.y;	z*=v.z;	return *this; }
	inline Vector3& operator/=(float f)					{ x/=f;		y/=f;	z/=f;	return *this; }
	inline Vector3& operator/=(const Vector3 &v)		{ x/=v.x;	y/=v.y;	z/=v.z; return *this; }

	inline Vector3 operator+(const Vector3 &v) const	{ Vector3 t; t.x = x+v.x;	t.y = y+v.y;	t.z = z+v.z;	return t; }
	inline Vector3 operator-(const Vector3 &v) const	{ Vector3 t; t.x = x-v.x;	t.y = y-v.y;	t.z = z-v.z;	return t; }
	inline Vector3 operator*(float f) const				{ Vector3 t; t.x = x*f;		t.y = y*f;		t.z = z*f;		return t; }
	inline Vector3 operator*(const Vector3 &v) const	{ Vector3 t; t.x = x*v.x;	t.y = y*v.y;	t.z = z*v.z;	return t; }
	inline Vector3 operator/(float f) const				{ Vector3 t; t.x = x/f;		t.y = y/f;		t.z = z/f;		return t; }
	inline Vector3 operator/(const Vector3 &v) const	{ Vector3 t; t.x = x/v.x;	t.y = y/v.y;	t.z = z/v.z;	return t; }

	inline Vector4& ToVector4()							{ return *(Vector4*)this; }
	inline Vector4& ToVector4()	const					{ return *(Vector4*)this; }

	inline operator float*()							{ return (float*)this; }
	inline operator float*() const						{ return (float*)this; }

	inline float Dot(const Vector3 &vec) const { return x*vec.x + y*vec.y + z*vec.z; }
	inline float MagSquared() const { return x*x + y*y + z*z; }
	inline float Magnitude() const { return MFSqrt(x*x + y*y + z*z); }
	inline Vector3& Normalise() { float l = MFSqrt(x*x + y*y + z*z); *this /= l; return *this; }
	inline Vector3 Lerp(const Vector3 &v, float t) const { return *this + (v-*this)*t; }
	inline Vector3 Cross(const Vector3 &v) const { Vector3 t; t.x=y*v.z-v.y*z; t.y=z*v.x-v.z*x; t.z=x*v.y-v.x*y; return t; }
	inline Vector3& Cross(const Vector3 &v, const Vector3 &v2) { x = v.y*v2.z - v.z*v2.y; y = v.z*v2.x - v.x*v2.z; z = v.x*v2.y - v.y*v2.x; return *this; }
	inline float Cross2(const Vector3 &v) const { return x*v.y - y*v.x; }
	inline float Distance(const Vector3 &v) const { return (v-*this).Magnitude(); }
	inline float GetAngle(const Vector3 &ref = up) { float d = MFACos(Dot(ref)); return (Cross2(ref) >= 0.0f) ? d : 2.0f*PI - d; }

	inline char* ToString() const { return STR("%.2f, %.2f, %.2f", x, y, z); }

	Vector3& ApplyMatrix(const Matrix &mat);
	Vector3& ApplyMatrix3x3(const Matrix &mat);
};

inline Vector3 operator*(float f, Vector3 v) { v.x *= f; v.y *= f; v.z *= f; return v; }

inline Vector3 Vector(float x, float y, float z = 0.0f) { Vector3 t; t.x = x; t.y = y; t.z = z; return t; }

template <>
inline Vector3 Min(Vector3 a, Vector3 b) { Vector3 t; t.x = a.x < b.x ? a.x : b.x; t.y = a.y < b.y ? a.y : b.y; t.z = a.z < b.z ? a.z : b.z; return t; }
template <>
inline Vector3 Max(Vector3 a, Vector3 b) { Vector3 t; t.x = a.x > b.x ? a.x : b.x; t.y = a.y > b.y ? a.y : b.y; t.z = a.z > b.z ? a.z : b.z; return t; }

#endif
