module fuji.matrix;

public import fuji.vector;

struct MFMatrix
{
	union
	{
		float[16] m =
		[
			1, 0, 0, 0, 
			0, 1, 0, 0, 
			0, 0, 1, 0, 
			0, 0, 0, 1

		];
		struct
		{
			MFVector x;
			MFVector y;
			MFVector z;
			MFVector t;
		}
		MFVector[4] row;
	}
/+
	string toString() const /*pure nothrow*/
	{
		return text( "[ ", m[0],  ",", m[1],  ",", m[2],  ",", m[3],  ", ",
					m[4],  ",", m[5],  ",", m[6],  ",", m[7],  ", ",
					m[8],  ",", m[9],  ",", m[10], ",", m[11], ", ",
					m[12], ",", m[13], ",", m[14], ",", m[15], " ]" );
	}
+/

	MFMatrix opBinary( string op )( float s ) const pure 			if( op == "*" )
	{
		MFMatrix m = this;
		m.m[] *= s;
		return m;
	}
	MFMatrix opBinaryRight( string op )( float s ) const pure 	if( op == "*" )
	{
		MFMatrix m = this;
		m.m[] *= s;
		return m;
	}

	MFVector opBinary( string op )( MFVector v ) const pure nothrow		if( op == "*" )
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

	static immutable MFMatrix identity = MFMatrix.init;
}

// handy templates

template IsMatrix( M )
{
	enum bool IsMatrix = is( M == MFMatrix );
}


// *** HLSL style interface, future SIMD vector library will be more like this too ***

// all the combinations of matrix multiplies... i think this could be written with a LOT less code.
auto mul( T0, T1 )( ref const(T0) a, ref const(T1) b ) pure nothrow if( IsMatrix!T0 || IsMatrix!T1 )
{
	static if( is( a == MFMatrix ) && is( b == MFMatrix ) )
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
}
