#include "MFQuaternion.h"

inline void MFQuaternion::Set(float _x, float _y, float _z, float _w)
{
	x = _x;
	y = _y;
	z = _z;
	w = _w;
}

inline bool MFQuaternion::operator==(const MFQuaternion &q) const
{
	return x==q.x && y==q.y && z==q.z && w==q.w;
}

inline bool MFQuaternion::operator!=(const MFQuaternion &q) const
{
	return x!=q.x || y!=q.y || z!=q.z || w!=q.w;
}

inline MFQuaternion& MFQuaternion::operator=(const MFQuaternion &q)
{
	x=q.x;
	y=q.y;
	z=q.z;
	w=q.w;
	return *this;
}

inline MFQuaternion MFQuaternion::operator*(const MFQuaternion &q) const
{
	MFQuaternion t;
	t.Multiply(*this, q);
	return t;
}

inline MFQuaternion::operator float*()
{
	return (float*)this;
}

inline MFQuaternion::operator const float*() const
{
	return (const float*)this;
}

inline MFQuaternion& MFQuaternion::Multiply(const MFQuaternion &q)
{
	return Multiply(*this, q);
}

inline MFQuaternion& MFQuaternion::Multiply(const MFQuaternion &q1, const MFQuaternion &q2)
{
	MFQuaternion r;

	r.w = q1.w*q2.w - q1.x*q2.x - q1.y*q2.y - q1.z*q2.z;
	r.x = q1.w*q2.x + q1.x*q2.w + q1.y*q2.z - q1.z*q2.y;
	r.y = q1.w*q2.y + q1.y*q2.w + q1.z*q2.x - q1.x*q2.z;
	r.z = q1.w*q2.z + q1.z*q2.w + q1.x*q2.y - q1.y*q2.x;

	return *this = r;
}

inline MFQuaternion& MFQuaternion::Conjugate(const MFQuaternion &q)
{
	x = -q.x;
	y = -q.y;
	z = -q.z;
	return *this;
}

inline MFQuaternion& MFQuaternion::Conjugate()
{
	return Conjugate(*this);
}

inline MFQuaternion& MFQuaternion::SLerp(const MFQuaternion &q, float t)
{
	// slerp here
	return *this;
}


inline const char * MFQuaternion::ToString() const
{
	return MFStr("%.2f, %.2f, %.2f, %.2f", x, y, z, w);
}

inline MFQuaternion MakeQuat(float x, float y, float z, float w)
{
	MFQuaternion t;
	t.x = x;
	t.y = y;
	t.z = z;
	t.w = w;
	return t;
}
