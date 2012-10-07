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

inline MFQuaternion MFQuaternion::operator+(const MFQuaternion &q) const
{
	MFQuaternion t;
	t.x = x + q.x;
	t.y = y + q.y;
	t.z = z + q.z;
	t.w = w + q.w;
	return t;
}

inline MFQuaternion MFQuaternion::operator-(const MFQuaternion &q) const
{
	MFQuaternion t;
	t.x = x - q.x;
	t.y = y - q.y;
	t.z = z - q.z;
	t.w = w - q.w;
	return t;
}

inline MFQuaternion MFQuaternion::operator*(float f) const
{
	MFQuaternion t;
	t.x = x*f;
	t.y = y*f;
	t.z = z*f;
	t.w = w*f;
	return t;
}

inline MFQuaternion MFQuaternion::operator*(const MFQuaternion &q) const
{
	MFQuaternion t;
	t.Multiply(*this, q);
	return t;
}

inline MFQuaternion& MFQuaternion::operator+=(const MFQuaternion &q)
{
	x += q.x;
	y += q.y;
	z += q.z;
	w += q.w;
	return *this;
}

inline MFQuaternion& MFQuaternion::operator-=(const MFQuaternion &q)
{
	x -= q.x;
	y -= q.y;
	z -= q.z;
	w -= q.w;
	return *this;
}

inline MFQuaternion& MFQuaternion::operator*=(float f)
{
	x *= f;
	y *= f;
	z *= f;
	w *= f;
	return *this;
}

inline MFQuaternion& MFQuaternion::operator*=(const MFQuaternion &q)
{
	return Multiply(*this, q);
}

inline MFQuaternion::operator float*()
{
	return (float*)this;
}

inline MFQuaternion::operator const float*() const
{
	return (const float*)this;
}

inline float MFQuaternion::Dot(const MFQuaternion &q) const
{
	return x*q.x + y*q.y + z*q.z + w*q.w;
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
	w = q.w;
	return *this;
}

inline MFQuaternion& MFQuaternion::Conjugate()
{
	x = -x;
	y = -y;
	z = -z;
	return *this;
}

inline MFQuaternion& MFQuaternion::Slerp(const MFQuaternion &q1, const MFQuaternion &q2, float t)
{
	MFQuaternion qt1 = q1;
	MFQuaternion qt2 = q2;

	float a = qt1.Dot(qt2);

	if(a < 0.0f) 
	{
		qt1 *= -1.0f;
		a *= -1.0f;
	}

	float scale;
	float invscale;

	if((a + 1.0f) > 0.05f) 
	{
		if ((1.0f - a) >= 0.05f)  // spherical interpolation
		{
			float theta = MFACos(a);
			float invsintheta = 1.0f / MFSin(theta);
			scale = MFSin(theta * (1.0f-t)) * invsintheta;
			invscale = MFSin(theta * t) * invsintheta;
		}
		else // linear interploation
		{
			scale = 1.0f - t;
			invscale = t;
		}
	}
	else
	{
		qt2.Set(-qt1.y, qt1.x, -qt1.w, qt1.z);
		scale = MFSin(MFPI * (0.5f - t));
		invscale = MFSin(MFPI * t);
	}

	return *this = (qt1*scale) + (qt2*invscale);
}

inline MFQuaternion& MFQuaternion::Slerp(const MFQuaternion &q, float t)
{
	return Slerp(*this, q, t);
}

inline MFVector MFQuaternion::Apply(const MFVector &v) const
{
	MFVector uv, uuv;
	MFVector qvec;
	qvec.Set(x, y, z);
	uv = qvec.Cross3(v);
	uuv = qvec.Cross3(uv);
	uv *= (2.0f * w);
	uuv *= 2.0f;

	return v + uv + uuv;
}

inline const char * MFQuaternion::ToString() const
{
	return MFStr("( %.2f, %.2f, %.2f ) %.2f", x, y, z, w);
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
