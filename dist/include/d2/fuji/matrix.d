module fuji.matrix;

public import fuji.vector;
import std.math;

struct MFMatrix
{
//	union
//	{
		struct
		{
			MFVector x = MFVector(1, 0, 0, 0);
			MFVector y = MFVector(0, 1, 0, 0);
			MFVector z = MFVector(0, 0, 1, 0);
			MFVector t = MFVector(0, 0, 0, 1);
		}
//		float[16] m;
//		MFVector[4] row = void;
//	}
/+
	string toString() const /*pure nothrow*/
	{
		return text( "[ ", m[0],  ",", m[1],  ",", m[2],  ",", m[3],  ", ",
					m[4],  ",", m[5],  ",", m[6],  ",", m[7],  ", ",
					m[8],  ",", m[9],  ",", m[10], ",", m[11], ", ",
					m[12], ",", m[13], ",", m[14], ",", m[15], " ]" );
	}
+/

	MFMatrix opBinary( string op )( float s ) const pure			if( op == "*" )
	{
		MFMatrix m = this;
		m.m[] *= s;
		return m;
	}
	MFMatrix opBinaryRight( string op )( float s ) const pure		if( op == "*" )
	{
		MFMatrix m = this;
		m.m[] *= s;
		return m;
	}

	MFVector opBinary( string op )( MFVector v ) const pure nothrow	if( op == "*" )
	{
		MFVector r = void;
		r.x = v.x*m[0] + v.y*m[4] + v.z*m[8]  + v.w*m[12];
		r.y = v.x*m[1] + v.y*m[5] + v.z*m[9]  + v.w*m[13];
		r.z = v.x*m[2] + v.y*m[6] + v.z*m[10] + v.w*m[14];
		r.w = v.x*m[3] + v.y*m[7] + v.z*m[11] + v.w*m[15];
		return r;
	}

	MFMatrix opBinary( string op )( ref const(MFMatrix) m ) const pure nothrow	if( op == "*" )
	{
		return mul( this, m );
	}

	MFMatrix opOpAssign( string op )( ref const(MFMatrix) m ) pure nothrow		if( op == "*=" )
	{
		this = mul( this, m );
		return this;
	}
/*
	MFMatrix SetTranslation(ref const MFVector trans)
	{
		m[1] = m[2] = m[3] = m[4] = m[6] = m[7] = m[8] = m[9] = m[11] = 0;
		m[0] = m[5] = m[10] = m[15] = 1;
		m[12] = trans.x;
		m[13] = trans.y;
		m[14] = trans.z;
		return this;
	}

	MFMatrix SetRotationX(float angle)
	{
		m[0] = 1;
		m[1] = 0;
		m[2] = 0;
		m[4] = 0;
		m[5] = cos(angle);
		m[6] = sin(angle);
		m[8] = 0;
		m[9] = -sin(angle);
		m[10] = cos(angle);
		m[12] = m[13] = m[14] = 0;
		m[15] = 1;
		return this;
	}

	MFMatrix SetRotationY(float angle)
	{
		m[0] = cos(angle);
		m[1] = 0;
		m[2] = -sin(angle);
		m[4] = 0;
		m[5] = 1;
		m[6] = 0;
		m[8] = sin(angle);
		m[9] = 0;
		m[10] = cos(angle);
		m[12] = m[13] = m[14] = 0;
		m[15] = 1;
		return this;
	}

	MFMatrix SetRotationZ(float angle)
	{
		m[0] = cos(angle);
		m[1] = sin(angle);
		m[2] = 0;
		m[4] = -sin(angle);
		m[5] = cos(angle);
		m[6] = 0;
		m[8] = 0;
		m[9] = 0;
		m[10] = 1;
		m[12] = m[13] = m[14] = 0;
		m[15] = 1;
		return this;
	}

	MFMatrix SetScale(ref const MFVector scale)
	{
		m[0] = scale.x;
		m[1] = m[2] = m[3] = m[4] = 0.0f;
		m[5] = scale.y;
		m[6] = m[7] = m[8] = m[9] = 0.0f;
		m[10] = scale.z;
		m[11] = m[12] = m[13] = m[14] = 0.0f;
		m[15] = 1.0f;
		return this;
	}

	MFMatrix RotateY(float angle)
	{
		MFMatrix rot;
		rot.SetRotationY(angle);
		return this.mul(rot);
	}
*/
	static immutable MFMatrix identity = MFMatrix.init;
}

// handy templates

template IsMatrix( M )
{
	enum bool IsMatrix = is( M == MFMatrix );
}


// *** HLSL style interface, future SIMD vector library will be more like this too ***

// all the combinations of matrix multiplies... i think this could be written with a LOT less code.
auto mul( T0, T1 )( ref const(T0) m, const(T1) v ) pure nothrow if( IsMatrix!T0 && IsVector!T1 )
{
	static if( is( std.traits.Unqual!(typeof(m)) == MFMatrix ) && is( std.traits.Unqual!(typeof(v)) == MFVector ) )
	{
		MFVector r;
		r.x = m.x.x*v.x + m.y.x*v.y + m.z.x*v.z + m.t.x*v.w;
		r.y = m.x.y*v.x + m.y.y*v.y + m.z.y*v.z + m.t.y*v.w;
		r.z = m.x.z*v.x + m.y.z*v.y + m.z.z*v.z + m.t.z*v.w;
		r.w = m.x.w*v.x + m.y.w*v.y + m.z.w*v.z + m.t.w*v.w;
		return r;
	}
	else
	{
		static assert(0);
	}
}

auto mul( T0, T1 )( ref const(T0) a, ref const(T1) b ) pure nothrow if( IsMatrix!T0 && IsMatrix!T1 )
{
	static if( is( std.traits.Unqual!(typeof(a)) == MFMatrix ) && is( std.traits.Unqual!(typeof(b)) == MFMatrix ) )
	{
		MFMatrix m = void;
		m.m[0]  = a.m[0]*b.m[0]  + a.m[1]*b.m[4]  + a.m[2]*b.m[8]   + a.m[3]*b.m[12];
		m.m[1]  = a.m[0]*b.m[1]  + a.m[1]*b.m[5]  + a.m[2]*b.m[9]   + a.m[3]*b.m[13];
		m.m[2]  = a.m[0]*b.m[2]  + a.m[1]*b.m[6]  + a.m[2]*b.m[10]  + a.m[3]*b.m[14];
		m.m[3]  = a.m[0]*b.m[3]  + a.m[1]*b.m[7]  + a.m[2]*b.m[11]  + a.m[3]*b.m[15];
		m.m[4]  = a.m[4]*b.m[0]  + a.m[5]*b.m[4]  + a.m[6]*b.m[8]   + a.m[7]*b.m[12];
		m.m[5]  = a.m[4]*b.m[1]  + a.m[5]*b.m[5]  + a.m[6]*b.m[9]   + a.m[7]*b.m[13];
		m.m[6]  = a.m[4]*b.m[2]  + a.m[5]*b.m[6]  + a.m[6]*b.m[10]  + a.m[7]*b.m[14];
		m.m[7]  = a.m[4]*b.m[3]  + a.m[5]*b.m[7]  + a.m[6]*b.m[11]  + a.m[7]*b.m[15];
		m.m[8]  = a.m[8]*b.m[0]  + a.m[9]*b.m[4]  + a.m[10]*b.m[8]  + a.m[11]*b.m[12];
		m.m[9]  = a.m[8]*b.m[1]  + a.m[9]*b.m[5]  + a.m[10]*b.m[9]  + a.m[11]*b.m[13];
		m.m[10] = a.m[8]*b.m[2]  + a.m[9]*b.m[6]  + a.m[10]*b.m[10] + a.m[11]*b.m[14];
		m.m[11] = a.m[8]*b.m[3]  + a.m[9]*b.m[7]  + a.m[10]*b.m[11] + a.m[11]*b.m[15];
		m.m[12] = a.m[12]*b.m[0] + a.m[13]*b.m[4] + a.m[14]*b.m[8]  + a.m[15]*b.m[12];
		m.m[13] = a.m[12]*b.m[1] + a.m[13]*b.m[5] + a.m[14]*b.m[9]  + a.m[15]*b.m[13];
		m.m[14] = a.m[12]*b.m[2] + a.m[13]*b.m[6] + a.m[14]*b.m[10] + a.m[15]*b.m[14];
		m.m[15] = a.m[12]*b.m[3] + a.m[13]*b.m[7] + a.m[14]*b.m[11] + a.m[15]*b.m[15];
		return m;
	}
/+
	else static if( is( a == MFMatrix ) && is( b == MFMatrixx3 ) )
	{
		MFMatrix m = void;
		m.m[0]  = a.m[0]*b.m[0]  + a.m[1]*b.m[3]  + a.m[2]*b.m[6]  + a.m[3]*b.m[9];
		m.m[1]  = a.m[0]*b.m[1]  + a.m[1]*b.m[4]  + a.m[2]*b.m[7]  + a.m[3]*b.m[10];
		m.m[2]  = a.m[0]*b.m[2]  + a.m[1]*b.m[5]  + a.m[2]*b.m[8]  + a.m[3]*b.m[11];
		m.m[3]  = a.m[3];
		m.m[4]  = a.m[4]*b.m[0]  + a.m[5]*b.m[3]  + a.m[6]*b.m[6]  + a.m[7]*b.m[9];
		m.m[5]  = a.m[4]*b.m[1]  + a.m[5]*b.m[4]  + a.m[6]*b.m[7]  + a.m[7]*b.m[10];
		m.m[6]  = a.m[4]*b.m[2]  + a.m[5]*b.m[5]  + a.m[6]*b.m[8]  + a.m[7]*b.m[11];
		m.m[7]  = a.m[7];
		m.m[8]  = a.m[8]*b.m[0]  + a.m[9]*b.m[3]  + a.m[10]*b.m[6] + a.m[11]*b.m[9];
		m.m[9]  = a.m[8]*b.m[1]  + a.m[9]*b.m[4]  + a.m[10]*b.m[7] + a.m[11]*b.m[10];
		m.m[10] = a.m[8]*b.m[2]  + a.m[9]*b.m[5]  + a.m[10]*b.m[8] + a.m[11]*b.m[11];
		m.m[11] = a.m[11];
		m.m[12] = a.m[12]*b.m[0] + a.m[13]*b.m[3] + a.m[14]*b.m[6] + a.m[15]*b.m[9];
		m.m[13] = a.m[12]*b.m[1] + a.m[13]*b.m[4] + a.m[14]*b.m[7] + a.m[15]*b.m[10];
		m.m[14] = a.m[12]*b.m[2] + a.m[13]*b.m[5] + a.m[14]*b.m[8] + a.m[15]*b.m[11];
		m.m[15] = a.m[15];
		return m;
	}
	else static if( is( a == MFMatrixx3 ) && is( b == MFMatrix ) )
	{
		MFMatrix m = void;
		m.m[0]  = a.m[0]*b.m[0] + a.m[1]*b.m[4]  + a.m[2]*b.m[8];
		m.m[1]  = a.m[0]*b.m[1] + a.m[1]*b.m[5]  + a.m[2]*b.m[9];
		m.m[2]  = a.m[0]*b.m[2] + a.m[1]*b.m[6]  + a.m[2]*b.m[10];
		m.m[3]  = a.m[0]*b.m[3] + a.m[1]*b.m[7]  + a.m[2]*b.m[11];
		m.m[4]  = a.m[3]*b.m[0] + a.m[4]*b.m[4]  + a.m[5]*b.m[8];
		m.m[5]  = a.m[3]*b.m[1] + a.m[4]*b.m[5]  + a.m[5]*b.m[9];
		m.m[6]  = a.m[3]*b.m[2] + a.m[4]*b.m[6]  + a.m[5]*b.m[10];
		m.m[7]  = a.m[3]*b.m[3] + a.m[4]*b.m[7]  + a.m[5]*b.m[11];
		m.m[8]  = a.m[6]*b.m[0] + a.m[7]*b.m[4]  + a.m[8]*b.m[8];
		m.m[9]  = a.m[6]*b.m[1] + a.m[7]*b.m[5]  + a.m[8]*b.m[9];
		m.m[10] = a.m[6]*b.m[2] + a.m[7]*b.m[6]  + a.m[8]*b.m[10];
		m.m[11] = a.m[6]*b.m[3] + a.m[7]*b.m[7]  + a.m[8]*b.m[11];
		m.m[12] = a.m[9]*b.m[0] + a.m[10]*b.m[4] + a.m[11]*b.m[8]  + b.m[12];
		m.m[13] = a.m[9]*b.m[1] + a.m[10]*b.m[5] + a.m[11]*b.m[9]  + b.m[13];
		m.m[14] = a.m[9]*b.m[2] + a.m[10]*b.m[6] + a.m[11]*b.m[10] + b.m[14];
		m.m[15] = a.m[9]*b.m[3] + a.m[10]*b.m[7] + a.m[11]*b.m[11] + b.m[15];
		return m;
	}
	else static if( is( a == MFMatrix ) && is( b == Matrix3 ) )
	{
		MFMatrix m = void;
		m.m[0]  = a.m[0]*b.m[0]  + a.m[1]*b.m[3]  + a.m[2]*b.m[6];
		m.m[1]  = a.m[0]*b.m[1]  + a.m[1]*b.m[4]  + a.m[2]*b.m[7];
		m.m[2]  = a.m[0]*b.m[2]  + a.m[1]*b.m[5]  + a.m[2]*b.m[8];
		m.m[3]  = a.m[3];
		m.m[4]  = a.m[4]*b.m[0]  + a.m[5]*b.m[3]  + a.m[6]*b.m[6];
		m.m[5]  = a.m[4]*b.m[1]  + a.m[5]*b.m[4]  + a.m[6]*b.m[7];
		m.m[6]  = a.m[4]*b.m[2]  + a.m[5]*b.m[5]  + a.m[6]*b.m[8];
		m.m[7]  = a.m[7];
		m.m[8]  = a.m[8]*b.m[0]  + a.m[9]*b.m[3]  + a.m[10]*b.m[6];
		m.m[9]  = a.m[8]*b.m[1]  + a.m[9]*b.m[4]  + a.m[10]*b.m[7];
		m.m[10] = a.m[8]*b.m[2]  + a.m[9]*b.m[5]  + a.m[10]*b.m[8];
		m.m[11] = a.m[11];
		m.m[12] = a.m[12]*b.m[0] + a.m[13]*b.m[3] + a.m[14]*b.m[6];
		m.m[13] = a.m[12]*b.m[1] + a.m[13]*b.m[4] + a.m[14]*b.m[7];
		m.m[14] = a.m[12]*b.m[2] + a.m[13]*b.m[5] + a.m[14]*b.m[8];
		m.m[15] = a.m[15];
		return m;
	}
	else static if( is( a == Matrix3 ) && is( b == MFMatrix ) )
	{
		MFMatrix m = void;
		m.m[0]  = a.m[0]*b.m[0] + a.m[1]*b.m[4]  + a.m[2]*b.m[8];
		m.m[1]  = a.m[0]*b.m[1] + a.m[1]*b.m[5]  + a.m[2]*b.m[9];
		m.m[2]  = a.m[0]*b.m[2] + a.m[1]*b.m[6]  + a.m[2]*b.m[10];
		m.m[3]  = a.m[0]*b.m[3] + a.m[1]*b.m[7]  + a.m[2]*b.m[11];
		m.m[4]  = a.m[3]*b.m[0] + a.m[4]*b.m[4]  + a.m[5]*b.m[8];
		m.m[5]  = a.m[3]*b.m[1] + a.m[4]*b.m[5]  + a.m[5]*b.m[9];
		m.m[6]  = a.m[3]*b.m[2] + a.m[4]*b.m[6]  + a.m[5]*b.m[10];
		m.m[7]  = a.m[3]*b.m[3] + a.m[4]*b.m[7]  + a.m[5]*b.m[11];
		m.m[8]  = a.m[6]*b.m[0] + a.m[7]*b.m[4]  + a.m[8]*b.m[8];
		m.m[9]  = a.m[6]*b.m[1] + a.m[7]*b.m[5]  + a.m[8]*b.m[9];
		m.m[10] = a.m[6]*b.m[2] + a.m[7]*b.m[6]  + a.m[8]*b.m[10];
		m.m[11] = a.m[6]*b.m[3] + a.m[7]*b.m[7]  + a.m[8]*b.m[11];
		m.m[12] = b.m[12];
		m.m[13] = b.m[13];
		m.m[14] = b.m[14];
		m.m[15] = b.m[15];
		return m;
	}
	else static if( is( a == MFMatrixx3 ) && is( b == MFMatrixx3 ) )
	{
		MFMatrixx3 m = void;
		m.m[0]  = a.m[0]*b.m[0] + a.m[1]*b.m[3]  + a.m[2]*b.m[6];
		m.m[1]  = a.m[0]*b.m[1] + a.m[1]*b.m[4]  + a.m[2]*b.m[7];
		m.m[2]  = a.m[0]*b.m[2] + a.m[1]*b.m[5]  + a.m[2]*b.m[8];
		m.m[3]  = a.m[3]*b.m[0] + a.m[4]*b.m[3]  + a.m[5]*b.m[6];
		m.m[4]  = a.m[3]*b.m[1] + a.m[4]*b.m[4]  + a.m[5]*b.m[7];
		m.m[5]  = a.m[3]*b.m[2] + a.m[4]*b.m[5]  + a.m[5]*b.m[8];
		m.m[6]  = a.m[6]*b.m[0] + a.m[7]*b.m[3]  + a.m[8]*b.m[6];
		m.m[7]  = a.m[6]*b.m[1] + a.m[7]*b.m[4]  + a.m[8]*b.m[7];
		m.m[8]  = a.m[6]*b.m[2] + a.m[7]*b.m[5]  + a.m[8]*b.m[8];
		m.m[9]  = a.m[9]*b.m[0] + a.m[10]*b.m[3] + a.m[11]*b.m[6] + b.m[9];
		m.m[10] = a.m[9]*b.m[1] + a.m[10]*b.m[4] + a.m[11]*b.m[7] + b.m[10];
		m.m[11] = a.m[9]*b.m[2] + a.m[10]*b.m[5] + a.m[11]*b.m[8] + b.m[11];
		return m;
	}
	else static if( is( a == MFMatrixx3 ) && is( b == Matrix3 ) )
	{
		MFMatrixx3 m = void;
		m.m[0]  = a.m[0]*b.m[0] + a.m[1]*b.m[3]  + a.m[2]*b.m[6];
		m.m[1]  = a.m[0]*b.m[1] + a.m[1]*b.m[4]  + a.m[2]*b.m[7];
		m.m[2]  = a.m[0]*b.m[2] + a.m[1]*b.m[5]  + a.m[2]*b.m[8];
		m.m[3]  = a.m[3]*b.m[0] + a.m[4]*b.m[3]  + a.m[5]*b.m[6];
		m.m[4]  = a.m[3]*b.m[1] + a.m[4]*b.m[4]  + a.m[5]*b.m[7];
		m.m[5]  = a.m[3]*b.m[2] + a.m[4]*b.m[5]  + a.m[5]*b.m[8];
		m.m[6]  = a.m[6]*b.m[0] + a.m[7]*b.m[3]  + a.m[8]*b.m[6];
		m.m[7]  = a.m[6]*b.m[1] + a.m[7]*b.m[4]  + a.m[8]*b.m[7];
		m.m[8]  = a.m[6]*b.m[2] + a.m[7]*b.m[5]  + a.m[8]*b.m[8];
		m.m[9]  = a.m[9]*b.m[0] + a.m[10]*b.m[3] + a.m[11]*b.m[6];
		m.m[10] = a.m[9]*b.m[1] + a.m[10]*b.m[4] + a.m[11]*b.m[7];
		m.m[11] = a.m[9]*b.m[2] + a.m[10]*b.m[5] + a.m[11]*b.m[8];
		return m;
	}
	else static if( is( a == Matrix3 ) && is( b == MFMatrixx3 ) )
	{
		MFMatrixx3 m = void;
		m.m[0]  = a.m[0]*b.m[0] + a.m[1]*b.m[3]  + a.m[2]*b.m[6];
		m.m[1]  = a.m[0]*b.m[1] + a.m[1]*b.m[4]  + a.m[2]*b.m[7];
		m.m[2]  = a.m[0]*b.m[2] + a.m[1]*b.m[5]  + a.m[2]*b.m[8];
		m.m[3]  = a.m[3]*b.m[0] + a.m[4]*b.m[3]  + a.m[5]*b.m[6];
		m.m[4]  = a.m[3]*b.m[1] + a.m[4]*b.m[4]  + a.m[5]*b.m[7];
		m.m[5]  = a.m[3]*b.m[2] + a.m[4]*b.m[5]  + a.m[5]*b.m[8];
		m.m[6]  = a.m[6]*b.m[0] + a.m[7]*b.m[3]  + a.m[8]*b.m[6];
		m.m[7]  = a.m[6]*b.m[1] + a.m[7]*b.m[4]  + a.m[8]*b.m[7];
		m.m[8]  = a.m[6]*b.m[2] + a.m[7]*b.m[5]  + a.m[8]*b.m[8];
		m.m[9]  = b.m[9];
		m.m[10] = b.m[10];
		m.m[11] = b.m[11];
		return m;
	}
	else static if( is( a == Matrix3 ) && is( b == Matrix3 ) )
	{
		Matrix3 m = void;
		m.m[0]  = a.m[0]*b.m[0] + a.m[1]*b.m[3]  + a.m[2]*b.m[6];
		m.m[1]  = a.m[0]*b.m[1] + a.m[1]*b.m[4]  + a.m[2]*b.m[7];
		m.m[2]  = a.m[0]*b.m[2] + a.m[1]*b.m[5]  + a.m[2]*b.m[8];
		m.m[3]  = a.m[3]*b.m[0] + a.m[4]*b.m[3]  + a.m[5]*b.m[6];
		m.m[4]  = a.m[3]*b.m[1] + a.m[4]*b.m[4]  + a.m[5]*b.m[7];
		m.m[5]  = a.m[3]*b.m[2] + a.m[4]*b.m[5]  + a.m[5]*b.m[8];
		m.m[6]  = a.m[6]*b.m[0] + a.m[7]*b.m[3]  + a.m[8]*b.m[6];
		m.m[7]  = a.m[6]*b.m[1] + a.m[7]*b.m[4]  + a.m[8]*b.m[7];
		m.m[8]  = a.m[6]*b.m[2] + a.m[7]*b.m[5]  + a.m[8]*b.m[8];
		return m;
	}
+/
	else
	{
		static assert(0);
	}
}
