module fuji.c.MFQuaternion;

public import fuji.c.MFVector;
import fuji.c.MFMatrix;

import std.math;

pure:
nothrow:
@nogc:

align(16) struct MFQuaternion
{
	float x = 0;
	float y = 0;
	float z = 0;
	float w = 1;

	string toString() const
	{
		return std.conv.text("[ ( ", x, ", ", y, ", ", z, " ) ", w, " ]");
	}

pure: nothrow: @nogc:
	MFQuaternion opBinary(string op)(const MFQuaternion q) const	if(op == "+") { return MFQuaternion(x + q.x, y + q.y, z + q.z, w + q.w); }
	MFQuaternion opBinary(string op)(const MFQuaternion q) const	if(op == "-") { return MFQuaternion(x - q.x, y - q.y, z - q.z, w - q.w); }

	MFQuaternion opBinary(string op)(float f) const					if(op == "*") { return MFQuaternion(x * f, y * f, z * f, w * f); }
	MFQuaternion opBinary(string op)(const MFQuaternion q) const	if(op == "*") { return mul(this, q); }
	MFVector opBinary(string op)(const MFVector v) const			if(op == "*") { return mul(this, v); }
//	MFMatrix opBinary(string op)(const MFMatrix m) const			if(op == "*") { return mul(this, m); }

	MFQuaternion opBinaryRight(string op)(float f) const			if(op == "*") { return MFQuaternion(f * x, f * y, f * z, f * w); }
//	MFMatrix opBinaryRight(string op)(const MFMatrix m) const		if(op == "*") { return mul(m, this); }

	MFQuaternion opOpAssign(string op)(const MFQuaternion q)		if(op == "+") { this = this + q; return this; }
	MFQuaternion opOpAssign(string op)(const MFQuaternion q)		if(op == "-") { this = this - q; return this; }
	MFQuaternion opOpAssign(string op)(const MFQuaternion q)		if(op == "*") { this = mul(this, q); return this; }
	MFQuaternion opOpAssign(string op)(float f)						if(op == "*") { this = this * f; return this; }

	__gshared immutable MFQuaternion identity = MFQuaternion.init;
}

float dot(const MFQuaternion v1, const MFQuaternion v2)
{
	return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z + v1.w*v2.w;
}

MFQuaternion conjugate(const MFQuaternion q)
{
	MFQuaternion r = void;
	r.x = -q.x;
	r.y = -q.y;
	r.z = -q.z;
	r.w = q.w;
	return r;
}

MFQuaternion slerp(const MFQuaternion q1, const MFQuaternion q2, float t)
{
	MFQuaternion qt1 = q1;
	MFQuaternion qt2 = q2;

	float a = dot(qt1, qt2);
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
			float theta = acos(a);
			float invsintheta = 1.0f / sin(theta);
			scale = sin(theta * (1.0f-t)) * invsintheta;
			invscale = sin(theta * t) * invsintheta;
		}
		else // linear interploation
		{
			scale = 1.0f - t;
			invscale = t;
		}
	}
	else
	{
		qt2 = MFQuaternion(-qt1.y, qt1.x, -qt1.w, qt1.z);
		scale = sin(PI * (0.5f - t));
		invscale = sin(PI * t);
	}

	return qt1*scale + qt2*invscale;
}

// handy templates

template IsQuaternion(Q)
{
	enum bool IsQuaternion = is(std.traits.Unqual!Q == MFQuaternion);
}


// *** HLSL style interface, future SIMD vector library will be more like this too ***

// quat-vector, quat-quat, matrix-quat, quat-matrix
auto mul(T0, T1)(const T0 q, const T1 v) if(IsQuaternion!T0 && IsVector!T1)
{
	MFVector uv, uuv;
	MFVector qvec = void;
	qvec.x = q.x;
	qvec.y = q.y;
	qvec.z = q.z;
	uv = qvec.cross3(v);
	uuv = qvec.cross3(uv);
	uv *= (2.0f * w);
	uuv *= 2.0f;
	return v + uv + uuv;
}

auto mul(T0, T1)(const T0 q1, const T1 q2) if(IsQuaternion!T0 && IsQuaternion!T1)
{
	MFQuaternion r = void;
	r.w = q1.w*q2.w - q1.x*q2.x - q1.y*q2.y - q1.z*q2.z;
	r.x = q1.w*q2.x + q1.x*q2.w + q1.y*q2.z - q1.z*q2.y;
	r.y = q1.w*q2.y + q1.y*q2.w + q1.z*q2.x - q1.x*q2.z;
	r.z = q1.w*q2.z + q1.z*q2.w + q1.x*q2.y - q1.y*q2.x;
	return r;
}

auto mul(T0, T1)(const T0 m, const T1 q) if(IsMatrix!T0 && IsQuaternion!T1)
{
	assert(0, "Write me!");
}

auto mul(T0, T1)(const T0 m, const T1 q) if(IsQuaternion!T0 && IsMatrix!T1)
{
	assert(0, "Write me!");
}
