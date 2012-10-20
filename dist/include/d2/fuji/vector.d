module fuji.vector;

import fuji.matrix;
import std.math;
//import std.conv;


struct MFVector
{
	float x = 0;
	float y = 0;
	float z = 0;
	float w = 0;

//	mixin SwizzleFeature;
//	string toString() const /*pure nothrow*/								{ return text( "[ ", x, ", ", y, ", ", z, ", ", w, " ]" ); }

	bool opEquals( const MFVector v ) const pure nothrow					{ return x == v.x && y == v.y && z == v.z && w == v.w; }

	MFVector opUnary( string op )() const pure nothrow if( op == "+" )		{ return *this; /* this is a noop */ }
	MFVector opUnary( string op )() const pure nothrow if( op == "-" )		{ return MFVector( -x, -y, -z, -w ); }

	MFVector opBinary( string op )( const MFVector v ) const pure nothrow	if( op == "+" ) { return MFVector( x + v.x, y + v.y, z + v.z, w + v.w ); }
	MFVector opBinary( string op )( const MFVector v ) const pure nothrow	if( op == "-" ) { return MFVector( x - v.x, y - v.y, z - v.z, w - v.w ); }
	MFVector opBinary( string op )( const MFVector v ) const pure nothrow	if( op == "*" ) { return MFVector( x * v.x, y * v.y, z * v.z, w * v.w ); }
	MFVector opBinary( string op )( const MFVector v ) const pure nothrow	if( op == "/" ) { return MFVector( x / v.x, y / v.y, z / v.z, w / v.w ); }
	MFVector opBinary( string op )( const MFVector v ) const pure nothrow	if( op == "%" ) { return MFVector( x % v.x, y % v.y, z % v.z, w % v.w ); }
	MFVector opBinary( string op )( float f ) const pure nothrow			if( op == "*" ) { return MFVector( x * f, y * f, z * f, w * f ); }
	MFVector opBinary( string op )( float f ) const pure nothrow			if( op == "/" ) { return MFVector( x / f, y / f, z / f, w / f ); }
	MFVector opBinary( string op )( float f ) const pure nothrow			if( op == "%" ) { return MFVector( x % f, y % f, z % f, w % f ); }

	MFVector opBinaryRight( string op )( float f ) const pure nothrow		if( op == "*" ) { return MFVector( f * x, f * y, f * z, f * w ); }
	MFVector opBinaryRight( string op )( float f ) const pure nothrow		if( op == "/" ) { return MFVector( f / x, f / y, f / z, f / w ); }
	MFVector opBinaryRight( string op )( float f ) const pure nothrow		if( op == "%" ) { return MFVector( f % x, f % y, f % z, f % w ); }

	MFVector opOpAssign( string op )( const MFVector v ) pure nothrow		if( op == "+" ) { this = this + v; return this; }
	MFVector opOpAssign( string op )( const MFVector v ) pure nothrow		if( op == "-" ) { this = this - v; return this; }
	MFVector opOpAssign( string op )( const MFVector v ) pure nothrow		if( op == "*" ) { this = this * v; return this; }
	MFVector opOpAssign( string op )( const MFVector v ) pure nothrow		if( op == "/" ) { this = this / v; return this; }
	MFVector opOpAssign( string op )( const MFVector v ) pure nothrow		if( op == "%" ) { this = this % v; return this; }
	MFVector opOpAssign( string op )( float v ) pure nothrow				if( op == "*" ) { this = this * v; return this; }
	MFVector opOpAssign( string op )( float v ) pure nothrow				if( op == "/" ) { this = this / v; return this; }
	MFVector opOpAssign( string op )( float v ) pure nothrow				if( op == "%" ) { this = this % v; return this; }

	MFVector rcp() const pure nothrow					{ return 1.0 / this; }

	float dot2( const MFVector v ) const pure nothrow	{ return x * v.x + y * v.y; }
	float dot3( const MFVector v ) const pure nothrow	{ return x * v.x + y * v.y + z * v.z; }
	float doth( const MFVector v ) const pure nothrow	{ return x * v.x + y * v.y + z * v.z + w; }
	float dot4( const MFVector v ) const pure nothrow	{ return x * v.x + y * v.y + z * v.z + w * v.w; }

	float magSq2() const pure nothrow	{ return x * x + y * y; }
	float magSq3() const pure nothrow	{ return x * x + y * y + z * z; }
	float magSq4() const pure nothrow	{ return x * x + y * y + z * z + w * w; }

	float mag2() const pure nothrow		{ return x * x + y * y; }
	float mag3() const pure nothrow		{ return x * x + y * y + z * z; }
	float mag4() const pure nothrow		{ return x * x + y * y + z * z + w * w; }

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
}


// handy templates

template IsVector(V)
{
	enum bool IsVector = is(V == MFVector);
}

// *** HLSL style interface, future SIMD vector library will be more like this too ***

MFVector abs(MFVector v) pure nothrow
{
	MFVector r = void;
	r.x = v.x < 0 ? -v.x : v.x;
	r.y = v.y < 0 ? -v.y : v.y;
	r.z = v.z < 0 ? -v.z : v.z;
	r.w = v.w < 0 ? -v.w : v.w;
	return r;
}

MFVector min(MFVector v1, MFVector v2) pure nothrow
{
	MFVector r = void;
	r.x = v1.x < v2.x ? v1.x : v2.x;
	r.y = v1.y < v2.y ? v1.y : v2.y;
	r.z = v1.z < v2.z ? v1.z : v2.z;
	r.w = v1.w < v2.w ? v1.w : v2.w;
	return r;
}

MFVector max(MFVector v1, MFVector v2) pure nothrow
{
	MFVector r = void;
	r.x = v1.x > v2.x ? v1.x : v2.x;
	r.y = v1.y > v2.y ? v1.y : v2.y;
	r.z = v1.z > v2.z ? v1.z : v2.z;
	r.w = v1.w > v2.w ? v1.w : v2.w;
	return r;
}

MFVector clamp(int width = 4)(MFVector v, MFVector low, MFVector high) pure nothrow
{
	MFVector r = v;
	static if(width >= 2)
	{
		r.x = v.x < low.x ? low.x : ( v.x > high.x ? high.x : value.x );
		r.y = v.y < low.y ? low.y : ( v.y > high.y ? high.y : value.y );
	}
	static if(width >= 3)
		r.z = v.z < low.z ? low.z : ( v.z > high.z ? high.z : value.z );
	static if(width >= 4)
		r.w = v.w < low.w ? low.w : ( v.w > high.w ? high.w : value.w );
	return r;
}

MFVector saturate(int width = 4)(MFVector v) pure nothrow
{
	return clamp!width(v, MFVector.zero, MFVector.one);
}

MFVector madd(MFVector v1, MFVector v2, MFVector v3) pure nothrow
{
	return v1*v2 + v3;
}

MFVector lerp(int width)(MFVector v1, MFVector v2, float t) pure nothrow
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

MFVector lerp(int width)(MFVector v1, MFVector v2, MFVector t) pure nothrow
{
	MFVector r = v;
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

MFVector rcp(int width)(MFVector v) pure nothrow
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

MFVector sqrt(int width = 4)(MFVector v) pure nothrow
{
	MFVector r = v;
	static if(width >= 2)
	{
		r.x = sqrt(v.x);
		r.y = sqrt(v.y);
	}
	static if(width >= 3)
		r.z = sqrt(v.z);
	static if(width >= 4)
		r.w = sqrt(v.w);
	return r;
}

MFVector rsqrt(int width = 4)(MFVector v) pure nothrow
{
	MFVector r = v;
	static if(width >= 2)
	{
		r.x = 1.0 / sqrt(v.x);
		r.y = 1.0 / sqrt(v.y);
	}
	static if(width >= 3)
		r.z = 1.0 / sqrt(v.z);
	static if(width >= 4)
		r.w = 1.0 / sqrt(v.w);
	return r;
}

float lengthSq(int width = 3)(MFVector v1, MFVector v2) pure nothrow
{
	return dot!width(v, v);
}

float length(int width = 3)(MFVector v) pure nothrow
{
	return std.math.sqrt(dot!width(v, v));
}

float distance(int width = 3)(MFVector a, MFVector b) pure nothrow
{
	return length!width(b-a);
}

MFVector normalise(int width = 3)(MFVector v) pure nothrow
{
	return v * (1.0 / length!width( v ));
}

float dot(int width = 3)(MFVector v1, MFVector v2) pure nothrow
{
	static if(width == 2)
		return v1.x*v2.x + v1.y*v2.y;
	else static if(width == 3)
		return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
	else static if(width == 4)
		return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z + v1.w*v2.w;
	else
		static assert( 0, "Invalid number of dimensions!" );
}

float dot2(MFVector v1, MFVector v2) pure nothrow
{
	return v1.x*v2.x + v1.y*v2.y;
}

float dot3(MFVector v1, MFVector v2) pure nothrow
{
	return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}

float dot4(MFVector v1, MFVector v2) pure nothrow
{
	return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z + v1.w*v2.w;
}

float doth(MFVector v1, MFVector v2) pure nothrow
{
	return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z + v2.w;
}

MFVector cross3(MFVector v1, MFVector v2) pure nothrow
{
	MFVector r = void;
    r.x = v1.y * v2.z - v1.z * v2.y;
    r.y = v1.z * v2.x - v1.x * v2.z;
    r.z = v1.x * v2.y - v1.y * v2.x;
	r.w = 0;
	return r;
}

// scalar-scalar, scalar-vector, vector-scalar, vector-vector (matrix versions in matrix.d)
auto mul( T0, T1 )( T0 a, T1 b ) pure nothrow if( ( is( T0 == float ) || IsVector!T0 ) && ( is( T1 == float ) || IsVector!T1 ) )
{
	return a * b;
}


private:
/+
// handy mixin to support arbitrary vector swizzling
mixin template SwizzleFeature()
{
	auto opDispatch( string s )() const pure nothrow if( isValidSwizzleString!( s, numComponents ) )
	{
		return MFVector( getComponent!( s[0], this ), getComponent!( s[1], this ), getComponent!( s[2], this ), getComponent!( s[3], this ) );
	}
}

template getComponent( char c, alias v )
{
	static if( c == 'x' )		enum float getComponent = v.x;
	else static if( c == 'y' )	enum float getComponent = v.y;
	else static if( c == 'z' )	enum float getComponent = v.z;
	else static if( c == 'w' )	enum float getComponent = v.w;
	else static if( c == '0' )	enum float getComponent = 0.0;
	else static if( c == '1' )	enum float getComponent = 1.0;
	else static if( c == '2' )	enum float getComponent = 2.0;
	else static assert( false, "Invalid swizzle component: '" ~ c ~ "'" );
}

template isValidSwizzleString( string s, int numComponents )
{
	enum bool isValidSwizzleString = charsInString( s, "xyzw012" );
}

bool charsInString( string s, string t )
{
	static bool charInString( char c, string s )
	{
		foreach( _c; s )
		{
			if( c == _c )
				return true;
		}
		return false;
	}

	foreach( c; s )
	{
		if( !charInString( c, t ) )
			return false;
	}
	return true;
}
+/