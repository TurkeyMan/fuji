module fuji.vector;

import std.math;

template isValidSwizzleString(string s, int numComponents)
{
	template charInString(char c, string s)
	{
		static if(s.length == 0)
			enum charInString = false;
		else
			enum charInString = c == s[0] || charInString!(c, s[1..$]);
	}

	template charsInString(string s, string t)
	{
		static if(s.length == 0)
			enum charsInString = true;
		else
			enum charsInString = charInString!(s[0], t) && charsInString!(s[1..$], t);
	}

	enum isValidSwizzleString = s.length == numComponents && charsInString!(s, "xyzw012");
}

private template getComponent(char c, alias v)
{
	static if(c == 'x')			alias getComponent = v.x;
	else static if(c == 'y')	alias getComponent = v.y;
	else static if(c == 'z')	alias getComponent = v.z;
	else static if(c == 'w')	alias getComponent = v.w;
	else static if(c == '0')	enum float getComponent = 0;
	else static if(c == '1')	enum float getComponent = 1;
	else static if(c == '2')	enum float getComponent = 2;
	else static assert(false, "Invalid swizzle component: '" ~ c ~ "'");
}

struct MFVector
{
	float x = 0;
	float y = 0;
	float z = 0;
	float w = 0;

	string toString() const												{ return std.conv.text("[ ", x, ", ", y, ", ", z, ", ", w, " ]"); }

	bool opEquals(const MFVector v) const pure nothrow					{ return x == v.x && y == v.y && z == v.z && w == v.w; }

	MFVector opUnary(string op)() const pure nothrow if(op == "+")		{ return *this; /* this is a noop */ }
	MFVector opUnary(string op)() const pure nothrow if(op == "-")		{ return MFVector(-x, -y, -z, -w); }

	MFVector opBinary(string op)(const MFVector v) const pure nothrow	if(op == "+") { return MFVector(x + v.x, y + v.y, z + v.z, w + v.w); }
	MFVector opBinary(string op)(const MFVector v) const pure nothrow	if(op == "-") { return MFVector(x - v.x, y - v.y, z - v.z, w - v.w); }
	MFVector opBinary(string op)(const MFVector v) const pure nothrow	if(op == "*") { return MFVector(x * v.x, y * v.y, z * v.z, w * v.w); }
	MFVector opBinary(string op)(const MFVector v) const pure nothrow	if(op == "/") { return MFVector(x / v.x, y / v.y, z / v.z, w / v.w); }
	MFVector opBinary(string op)(const MFVector v) const pure nothrow	if(op == "%") { return MFVector(x % v.x, y % v.y, z % v.z, w % v.w); }
	MFVector opBinary(string op)(float f) const pure nothrow			if(op == "*") { return MFVector(x * f, y * f, z * f, w * f); }
	MFVector opBinary(string op)(float f) const pure nothrow			if(op == "/") { return MFVector(x / f, y / f, z / f, w / f); }
	MFVector opBinary(string op)(float f) const pure nothrow			if(op == "%") { return MFVector(x % f, y % f, z % f, w % f); }

	MFVector opBinaryRight(string op)(float f) const pure nothrow		if(op == "*") { return MFVector(f * x, f * y, f * z, f * w); }
	MFVector opBinaryRight(string op)(float f) const pure nothrow		if(op == "/") { return MFVector(f / x, f / y, f / z, f / w); }
	MFVector opBinaryRight(string op)(float f) const pure nothrow		if(op == "%") { return MFVector(f % x, f % y, f % z, f % w); }

	MFVector opOpAssign(string op)(const MFVector v) pure nothrow		if(op == "+") { return this = this + v; }
	MFVector opOpAssign(string op)(const MFVector v) pure nothrow		if(op == "-") { return this = this - v; }
	MFVector opOpAssign(string op)(const MFVector v) pure nothrow		if(op == "*") { return this = this * v; }
	MFVector opOpAssign(string op)(const MFVector v) pure nothrow		if(op == "/") { return this = this / v; }
	MFVector opOpAssign(string op)(const MFVector v) pure nothrow		if(op == "%") { return this = this % v; }
	MFVector opOpAssign(string op)(float f) pure nothrow				if(op == "*") { return this = this * f; }
	MFVector opOpAssign(string op)(float f) pure nothrow				if(op == "/") { return this = this / f; }
	MFVector opOpAssign(string op)(float f) pure nothrow				if(op == "%") { return this = this % f; }

	// handy mixin to support arbitrary vector swizzling
	auto opDispatch(string s)() const pure nothrow if(isValidSwizzleString!(s, 4))
	{
		return MFVector(getComponent!(s[0], this), getComponent!(s[1], this), getComponent!(s[2], this), getComponent!(s[3], this));
	}

	float magSq2() const pure nothrow	{ return x * x + y * y; }
	float magSq3() const pure nothrow	{ return x * x + y * y + z * z; }
	float magSq4() const pure nothrow	{ return x * x + y * y + z * z + w * w; }

	float mag2() const pure nothrow		{ return std.math.sqrt(x * x + y * y); }
	float mag3() const pure nothrow		{ return std.math.sqrt(x * x + y * y + z * z); }
	float mag4() const pure nothrow		{ return std.math.sqrt(x * x + y * y + z * z + w * w); }

	static immutable MFVector zero = MFVector(0,0,0,0);
	static immutable MFVector one = MFVector(1,1,1,1);
	static immutable MFVector right = MFVector(1,0,0,0);
	static immutable MFVector up = MFVector(0,1,0,0);
	static immutable MFVector forward = MFVector(0,0,1,0);
	static immutable MFVector origin = MFVector(0,0,0,0);
	static immutable MFVector identity = MFVector(0,0,0,1);

	static immutable MFVector black = MFVector(0,0,0,1);
	static immutable MFVector white = MFVector(1,1,1,1);
	static immutable MFVector red = MFVector(1,0,0,1);
	static immutable MFVector green = MFVector(0,1,0,1);
	static immutable MFVector blue = MFVector(0,0,1,1);
	static immutable MFVector cyan = MFVector(0,1,1,1);
	static immutable MFVector magenta = MFVector(1,0,1,1);
	static immutable MFVector yellow = MFVector(1,1,0,1);
	static immutable MFVector orange = MFVector(1,0.5,0,1);
}


// handy templates

template IsVector(V)
{
	enum bool IsVector = is(std.traits.Unqual!V == MFVector);
}

// *** HLSL style interface, future SIMD vector library will be more like this too ***

MFVector abs(const MFVector v) pure nothrow
{
	MFVector r = void;
	r.x = v.x < 0 ? -v.x : v.x;
	r.y = v.y < 0 ? -v.y : v.y;
	r.z = v.z < 0 ? -v.z : v.z;
	r.w = v.w < 0 ? -v.w : v.w;
	return r;
}

MFVector min(const MFVector v1, const MFVector v2) pure nothrow
{
	MFVector r = void;
	r.x = v1.x < v2.x ? v1.x : v2.x;
	r.y = v1.y < v2.y ? v1.y : v2.y;
	r.z = v1.z < v2.z ? v1.z : v2.z;
	r.w = v1.w < v2.w ? v1.w : v2.w;
	return r;
}

MFVector max(const MFVector v1, const MFVector v2) pure nothrow
{
	MFVector r = void;
	r.x = v1.x > v2.x ? v1.x : v2.x;
	r.y = v1.y > v2.y ? v1.y : v2.y;
	r.z = v1.z > v2.z ? v1.z : v2.z;
	r.w = v1.w > v2.w ? v1.w : v2.w;
	return r;
}

MFVector clamp(int width = 4)(const MFVector v, const MFVector low, const MFVector high) pure nothrow
{
	MFVector r = v;
	static if(width >= 2)
	{
		r.x = v.x < low.x ? low.x : (v.x > high.x ? high.x : value.x);
		r.y = v.y < low.y ? low.y : (v.y > high.y ? high.y : value.y);
	}
	static if(width >= 3)
		r.z = v.z < low.z ? low.z : (v.z > high.z ? high.z : value.z);
	static if(width >= 4)
		r.w = v.w < low.w ? low.w : (v.w > high.w ? high.w : value.w);
	return r;
}

MFVector saturate(int width = 4)(const MFVector v) pure nothrow
{
	return clamp!width(v, MFVector.zero, MFVector.one);
}

MFVector madd(const MFVector v1, const MFVector v2, const MFVector v3) pure nothrow
{
	return v1*v2 + v3;
}

MFVector lerp(int width = 4)(const MFVector v1, const MFVector v2, float t) pure nothrow
{
	MFVector r = v1;
	static if(width >= 2)
	{
		r.x = v1.x + (v2.x - v1.x)*t;
		r.y = v1.y + (v2.y - v1.y)*t;
	}
	static if(width >= 3)
		r.z = v1.z + (v2.z - v1.z)*t;
	static if(width >= 4)
		r.w = v1.w + (v2.w - v1.w)*t;
	return r;
}

MFVector lerp(int width = 4)(const MFVector v1, const MFVector v2, const MFVector t) pure nothrow
{
	MFVector r = v1;
	static if(width >= 2)
	{
		r.x = v1.x + (v2.x - v1.x)*t.x;
		r.y = v1.y + (v2.y - v1.y)*t.y;
	}
	static if(width >= 3)
		r.z = v1.z + (v2.z - v1.z)*t.z;
	static if(width >= 4)
		r.w = v1.w + (v2.w - v1.w)*t.w;
	return r;
}

MFVector rcp(int width = 4)(const MFVector v) pure nothrow
{
	MFVector r = v;
	static if(width >= 2)
	{
		r.x = 1.0 / v.x;
		r.y = 1.0 / v.y;
	}
	static if(width >= 3)
		r.z = 1.0 / v.z;
	static if(width >= 4)
		r.w = 1.0 / v.w;
	return r;
}

MFVector sqrt(int width = 4)(const MFVector v) pure nothrow
{
	MFVector r = v;
	static if(width >= 2)
	{
		r.x = std.math.sqrt(v.x);
		r.y = std.math.sqrt(v.y);
	}
	static if(width >= 3)
		r.z = std.math.sqrt(v.z);
	static if(width >= 4)
		r.w = std.math.sqrt(v.w);
	return r;
}

MFVector rsqrt(int width = 4)(const MFVector v) pure nothrow
{
	MFVector r = v;
	static if(width >= 2)
	{
		r.x = 1.0 / std.math.sqrt(v.x);
		r.y = 1.0 / std.math.sqrt(v.y);
	}
	static if(width >= 3)
		r.z = 1.0 / std.math.sqrt(v.z);
	static if(width >= 4)
		r.w = 1.0 / std.math.sqrt(v.w);
	return r;
}

float lengthSq(int width = 3)(const MFVector v) pure nothrow
{
	return dot!width(v, v);
}

float length(int width = 3)(const MFVector v) pure nothrow
{
	return std.math.sqrt(dot!width(v, v));
}

float distanceSq(int width = 3)(const MFVector a, const MFVector b) pure nothrow
{
	return lengthSq!width(b-a);
}

float distance(int width = 3)(const MFVector a, const MFVector b) pure nothrow
{
	return length!width(b-a);
}

MFVector normalise(int width = 3)(const MFVector v) pure nothrow
{
	return v * (1.0 / length!width(v));
}

float dot(int width = 3)(const MFVector v1, const MFVector v2) pure nothrow
{
	static if(width == 2)
		return v1.x*v2.x + v1.y*v2.y;
	else static if(width == 3)
		return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
	else static if(width == 4)
		return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z + v1.w*v2.w;
	else
		static assert(0, "Invalid number of dimensions!");
}

float dot2(const MFVector v1, const MFVector v2) pure nothrow
{
	return v1.x*v2.x + v1.y*v2.y;
}

float dot3(const MFVector v1, const MFVector v2) pure nothrow
{
	return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}

float dot4(const MFVector v1, const MFVector v2) pure nothrow
{
	return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z + v1.w*v2.w;
}

float doth(const MFVector v1, const MFVector v2) pure nothrow
{
	return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z + v2.w;
}

MFVector cross3(const MFVector v1, const MFVector v2) pure nothrow
{
	MFVector r = void;
    r.x = v1.y * v2.z - v1.z * v2.y;
    r.y = v1.z * v2.x - v1.x * v2.z;
    r.z = v1.x * v2.y - v1.y * v2.x;
	r.w = 0;
	return r;
}

// scalar-scalar, scalar-vector, vector-scalar, vector-vector (matrix versions in matrix.d)
auto mul(T0, T1)(const T0 a, const T1 b) pure nothrow if((is(T0 == float) || IsVector!T0) && (is(T1 == float) || IsVector!T1))
{
	return a * b;
}
