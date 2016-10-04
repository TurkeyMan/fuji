module fuji.c.MFMatrix;

public import fuji.c.MFVector;
import fuji.c.MFQuaternion;
static import fuji.vector;

import std.math;
import std.conv : text;

pure:
nothrow:
@nogc:

struct MFMatrix
{
	union
	{
		struct
		{
			MFVector x = MFVector(1, 0, 0, 0);
			MFVector y = MFVector(0, 1, 0, 0);
			MFVector z = MFVector(0, 0, 1, 0);
			MFVector t = MFVector(0, 0, 0, 1);
		}
		float[16] m = void;
		MFVector[4] row = void;
	}

	string toString() const
	{
		return text("[ ", x.toString(),  ", ", y.toString(),  ", ", z.toString(),  ", ", t.toString(),  " ]");
	}

pure: nothrow: @nogc:
	MFMatrix opBinary(string op)(float s) const							if(op == "*")	{ MFMatrix r = void; r.m[] = m[] * s; return r; }
	MFMatrix opBinaryRight(string op)(float s) const					if(op == "*")	{ MFMatrix r = void; r.m[] = m[] * s; return r; }
	MFMatrix opBinary(string op)(const(MFMatrix) m) const				if(op == "*")	{ return mul(this, m); }
	MFVector opBinary(string op)(const MFVector v) const				if(op == "*")	{ return mul(this, v); }
	MFMatrix opOpAssign(string op)(const(MFMatrix) m)					if(op == "*=")	{ return this = mul(this, m); }
	MFMatrix opOpAssign(string op)(float s)								if(op == "*=")	{ return this = this * s; }

	ref MFMatrix setTranslation(const MFVector trans)
	{
		m[12] = trans.x;
		m[13] = trans.y;
		m[14] = trans.z;
		m[15] = trans.w;
		return this;
	}

	ref MFMatrix setRotationX(float angle)
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
		m[3] = m[7] = m[11] = 0.0f;
		return this;
	}

	ref MFMatrix setRotationY(float angle)
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
		m[3] = m[7] = m[11] = 0.0f;
		return this;
	}

	ref MFMatrix setRotationZ(float angle)
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
		m[3] = m[7] = m[11] = 0.0f;
		return this;
	}

	ref MFMatrix setRotationYPR(float yaw, float pitch, float roll)
	{
		float cosy = cos(yaw);
		float siny = sin(yaw);
		float cosp = cos(pitch);
		float sinp = sin(pitch);
		float cosr = cos(roll);
		float sinr = sin(roll);

		m[0] =  cosr * cosy + sinp * sinr * siny;
		m[1] =  cosp * sinr;
		m[2] =  cosy * sinp * sinr - cosr * siny;

		m[4] = -cosy * sinr + cosr * sinp * siny;
		m[5] =  cosp * cosr;
		m[6] =  cosr * cosy * sinp + sinr * siny;

		m[8] =  cosp * siny;
		m[9] = -sinp;
		m[10] =  cosp * cosy;

		m[3] = m[7] = m[11] = 0.0f;
		return this;
	}

	ref MFMatrix setRotation(const MFVector axis, float angle)
	{
		// do the trig
		float s = sin(angle);
		float c = cos(angle);
		float t = 1.0f-c;

		// build the rotation matrix
		m[0] = t*axis.x*axis.x + c;
		m[4] = t*axis.x*axis.y - s*axis.z;
		m[8] = t*axis.x*axis.z + s*axis.y;

		m[1] = t*axis.x*axis.y + s*axis.z;
		m[5] = t*axis.y*axis.y + c;
		m[9] = t*axis.y*axis.z - s*axis.x;

		m[2] = t*axis.x*axis.z - s*axis.y;
		m[6] = t*axis.y*axis.z + s*axis.x;
		m[10] = t*axis.z*axis.z + c;

		m[3]= m[7] = m[11] = 0.0f;
		return this;
	}

	ref MFMatrix setRotationQ(const MFQuaternion q)
	{
		float xx = q.x*q.x;
		float xy = q.x*q.y;
		float xz = q.x*q.z;
		float xw = q.x*q.w;
		float yy = q.y*q.y;
		float yz = q.y*q.z;
		float yw = q.y*q.w;
		float zz = q.z*q.z;
		float zw = q.z*q.w;

		m[0] = 1.0f - 2.0f*(yy+zz);
		m[1] = 2.0f*(xy-zw);
		m[2] = 2.0f*(xz+yw);
		m[4] = 2.0f*(xy+zw);
		m[5] = 1.0f - 2.0f*(xx+zz);
		m[6] = 2.0f*(yz-xw);
		m[8] = 2.0f*(xz-yw);
		m[9] = 2.0f*(yz+xw);
		m[10] = 1.0f - 2.0f*(xx+yy);

		m[3] = m[7] = m[11] = 0.0f;
		return this;
	}

	MFQuaternion getRotationQ() const
	{
		MFQuaternion q;
		float trace = m[0] + m[5] + m[10] + 1.0f;

		if(trace > 0.0f)
		{
			float s = sqrt(trace);
			float invS = 0.5f / s;
			q.w = 0.5f * s;
			q.x = (m[9] - m[6]) * invS;
			q.y = (m[2] - m[8]) * invS;
			q.z = (m[4] - m[1]) * invS;
		}
		else
		{
			if(m[0] > m[5] && m[0] > m[10])
			{
				float s = sqrt(1.0f + m[0] - m[5] - m[10]);
				float invS = 0.5f / s;
				q.x = 0.5f * s;
				q.y = (m[1] + m[4]) * invS;
				q.z = (m[2] + m[8]) * invS;
				q.w = (m[6] - m[9]) * invS;
			}
			else if(m[5] > m[10])
			{
				float s = sqrt(1.0f + m[5] - m[0] - m[10]);
				float invS = 0.5f / s;
				q.x = (m[1] + m[4]) * invS;
				q.y = 0.5f * s;
				q.z = (m[6] + m[9]) * invS;
				q.w = (m[2] - m[8]) * invS;
			}
			else
			{
				float s = sqrt(1.0f + m[10] - m[0] - m[5]);
				float invS = 0.5f / s;
				q.x = (m[2] + m[8] ) * invS;
				q.y = (m[6] + m[9] ) * invS;
				q.z = 0.5f * s;
				q.w = (m[1] - m[4] ) * invS;
			}
		}

		return q;
	}

	ref MFMatrix setScale(MFVector scale)
	{
		m[0] = scale.x;
		m[1] = m[2] = m[3] = m[4] = 0.0f;
		m[5] = scale.y;
		m[6] = m[7] = m[8] = m[9] = 0.0f;
		m[10] = scale.z;
		m[11] = 0.0f;
		return this;
	}

	ref MFMatrix translate(MFVector trans)
	{
		t += trans;
		return this;
	}

	ref MFMatrix rotateX(float angle)
	{
		MFMatrix rot;
		rot.setRotationX(angle);
		return this = mul(this, rot);
	}

	ref MFMatrix rotateY(float angle)
	{
		MFMatrix rot;
		rot.setRotationY(angle);
		return this = mul(this, rot);
	}

	ref MFMatrix rotateZ(float angle)
	{
		MFMatrix rot;
		rot.setRotationZ(angle);
		return this = mul(this, rot);
	}

	ref MFMatrix scale(MFVector scale)
	{
		x *= scale.x;
		y *= scale.y;
		z *= scale.z;
		return this;
	}

	ref MFMatrix lookAt(MFVector pos, MFVector at, MFVector up = MFVector.up)
	{
		z = (at-pos).normalise();		// calculate forwards
		x = cross3(up, z).normalise();	// calculate right
		y = cross3(z, x);				// calculate up (z and x are already normalised)
		t = pos;						// set translation
		return this;
	}

	ref MFMatrix setPerspective(float fov, float near, float far, float aspectRatio)
	{
		// construct perspective projection
		float zn = near;
		float zf = far;

		float a = fov * 0.5f;

		float h = cos(a) / sin(a);
		float w = h / aspectRatio;

		float zd = zf-zn;
		float zs = zf/zd;

		version(_OPENGL_CLIP_SPACE)
		{
			m[0] = w;		m[1] = 0.0f;	m[2] = 0.0f;			m[3] = 0.0f;
			m[4] = 0.0f;	m[5] = h;		m[6] = 0.0f;			m[7] = 0.0f;
			m[8] = 0.0f;	m[9] = 0.0f;	m[10] = 2.0f*zs;		m[11] = 1.0f;
			m[12] = 0.0f;	m[13] = 0.0f;	m[14] = -2.0f*zn*zs-zf;	m[15] = 0.0f;
		}
		else
		{
			m[0] = w;		m[1] = 0.0f;	m[2] = 0.0f;			m[3] = 0.0f;
			m[4] = 0.0f;	m[5] = h;		m[6] = 0.0f;			m[7] = 0.0f;
			m[8] = 0.0f;	m[9] = 0.0f;	m[10] = zs;				m[11] = 1.0f;
			m[12] = 0.0f;	m[13] = 0.0f;	m[14] = -zn*zs;			m[15] = 0.0f;
		}

		return this;
	}

	ref MFMatrix setOrthographic(float top, float left, float bottom, float right, float near = 0, float far = 1)
	{
		m[0] = 2.0f/(right-left);			m[1] = 0.0f;						m[2] = 0.0f;				m[3] = 0.0f;
		m[4] = 0.0f;						m[5] = 2.0f/(top-bottom);			m[6] = 0.0f;				m[7] = 0.0f;
		m[8] = 0.0f;						m[9] = 0.0f;						m[10] = 1.0f/(far-near);	m[11] = 0.0f;
		m[12] = (left+right)/(left-right);	m[13] = (top+bottom)/(bottom-top);	m[14] = near/(near-far);	m[15] = 1.0f;
		return this;
	}

	ref MFMatrix normalise()
	{
		x = x.normalise();
		y = y.normalise();
		z = z.normalise();
		return this;
	}

	ref MFMatrix transpose()
	{
		float t;
		t=m[4]; m[4]=m[1]; m[1]=t;
		t=m[8]; m[8]=m[2]; m[2]=t;
		t=m[9]; m[9]=m[6]; m[6]=t;
		t=m[12]; m[12]=m[3]; m[3]=t;
		t=m[13]; m[13]=m[7]; m[7]=t;
		t=m[14]; m[14]=m[11]; m[11]=t;
		return this;
	}

	ref MFMatrix transpose3x3()
	{
		float t;
		t=m[4]; m[4]=m[1]; m[1]=t;
		t=m[8]; m[8]=m[2]; m[2]=t;
		t=m[9]; m[9]=m[6]; m[6]=t;
		return this;
	}

	static immutable MFMatrix identity = MFMatrix.init;
}

MFMatrix lerp(const(MFMatrix) start, const(MFMatrix) end, float time)
{
	MFMatrix t = void;
	t.x = fuji.vector.lerp(start.x, end.x, time);
	t.y = fuji.vector.lerp(start.y, end.y, time);
	t.z = fuji.vector.lerp(start.z, end.z, time);
	t.t = fuji.vector.lerp(start.t, end.t, time);
	return t;
}

MFMatrix inverse(const(MFMatrix) matrix)
{
	enum float PRECISION_LIMIT = 1.0e-10;

	MFMatrix inv;
	float det_1;
	float pos, neg, temp;

	//	* Calculate the determinant of submatrix A and determine if the
	//	* the matrix is singular as limited by the single precision
	//	* floating-point data representation.
	pos = neg = 0.0;
	temp =  matrix.m[0]*matrix.m[5]*matrix.m[10];
	if(temp >= 0.0) pos += temp; else neg += temp;
	temp =  matrix.m[1]*matrix.m[6]*matrix.m[8];
	if(temp >= 0.0) pos += temp; else neg += temp;
	temp =  matrix.m[2]*matrix.m[4]*matrix.m[9];
	if(temp >= 0.0) pos += temp; else neg += temp;
	temp = -matrix.m[2]*matrix.m[5]*matrix.m[8];
	if(temp >= 0.0) pos += temp; else neg += temp;
	temp = -matrix.m[1]*matrix.m[4]*matrix.m[10];
	if(temp >= 0.0) pos += temp; else neg += temp;
	temp = -matrix.m[0]*matrix.m[6]*matrix.m[9];
	if(temp >= 0.0) pos += temp; else neg += temp;
	det_1 = pos + neg;

	// Is the submatrix A singular?
	if((det_1 == 0) || (abs(det_1 / (pos - neg)) < PRECISION_LIMIT))
	{
		// MFMatrix M has no inverse
//		debug fuji.dbg.MFDebug_Warn(3, "MFMatrix::Inverse: Singular matrix (Matrix has no inverse)...\n");
		return matrix;
	}

	// Calculate inverse(A) = adj(A) / det(A)
	det_1 = 1.0f / det_1;
	inv.m[0]  =  (matrix.m[5]*matrix.m[10] - matrix.m[6]*matrix.m[9]) * det_1;
	inv.m[4]  = -(matrix.m[4]*matrix.m[10] - matrix.m[6]*matrix.m[8]) * det_1;
	inv.m[8]  =  (matrix.m[4]*matrix.m[9]  - matrix.m[5]*matrix.m[8]) * det_1;
	inv.m[1]  = -(matrix.m[1]*matrix.m[10] - matrix.m[2]*matrix.m[9]) * det_1;
	inv.m[5]  =  (matrix.m[0]*matrix.m[10] - matrix.m[2]*matrix.m[8]) * det_1;
	inv.m[9]  = -(matrix.m[0]*matrix.m[9]  - matrix.m[1]*matrix.m[8]) * det_1;
	inv.m[2]  =  (matrix.m[1]*matrix.m[6]  - matrix.m[2]*matrix.m[5]) * det_1;
	inv.m[6]  = -(matrix.m[0]*matrix.m[6]  - matrix.m[2]*matrix.m[4]) * det_1;
	inv.m[10] =  (matrix.m[0]*matrix.m[5]  - matrix.m[1]*matrix.m[4]) * det_1;

	// Calculate -C * inverse(A)
	inv.m[12] = -(matrix.m[12]*inv.m[0] + matrix.m[13]*inv.m[4] + matrix.m[14]*inv.m[8]);
	inv.m[13] = -(matrix.m[12]*inv.m[1] + matrix.m[13]*inv.m[5] + matrix.m[14]*inv.m[9]);
	inv.m[14] = -(matrix.m[12]*inv.m[2] + matrix.m[13]*inv.m[6] + matrix.m[14]*inv.m[10]);

	// Fill in last column
	inv.m[3]  = inv.m[7] = inv.m[11] = 0.0f;
	inv.m[15] = 1.0f;

	return inv;
}

MFVector transformVector(const(MFMatrix) matrix, const(MFVector) vector)
{
	MFVector t;
	t.x = vector.x*matrix.m[0] + vector.y*matrix.m[4] + vector.z*matrix.m[8] + vector.w*matrix.m[12];
	t.y = vector.x*matrix.m[1] + vector.y*matrix.m[5] + vector.z*matrix.m[9] + vector.w*matrix.m[13];
	t.z = vector.x*matrix.m[2] + vector.y*matrix.m[6] + vector.z*matrix.m[10] + vector.w*matrix.m[14];
	t.w = vector.x*matrix.m[3] + vector.y*matrix.m[7] + vector.z*matrix.m[11] + vector.w*matrix.m[15];
	return t;
}

MFVector transformVectorH(const(MFMatrix) matrix, const(MFVector) vector)
{
	MFVector t;
/+
	t = madd3(matrix.x, vector.x, matrix.t);
	t = madd3(matrix.y, vector.y, t);
	t = madd3(matrix.z, vector.z, t);
+/
	t.x = vector.x*matrix.m[0] + vector.y*matrix.m[4] + vector.z*matrix.m[8] + matrix.m[12];
	t.y = vector.x*matrix.m[1] + vector.y*matrix.m[5] + vector.z*matrix.m[9] + matrix.m[13];
	t.z = vector.x*matrix.m[2] + vector.y*matrix.m[6] + vector.z*matrix.m[10] + matrix.m[14];
	return t;
}

MFVector transformVector3(const(MFMatrix) matrix, const(MFVector) vector)
{
	MFVector t;
	t.x = vector.x*matrix.m[0] + vector.y*matrix.m[4] + vector.z*matrix.m[8];
	t.y = vector.x*matrix.m[1] + vector.y*matrix.m[5] + vector.z*matrix.m[9];
	t.z = vector.x*matrix.m[2] + vector.y*matrix.m[6] + vector.z*matrix.m[10];
	return t;
}



// handy templates

enum IsMatrix(T) = is(std.traits.Unqual!T == MFMatrix);


// *** HLSL style interface, future SIMD vector library will be more like this too ***

// all the combinations of matrix multiplies... i think this could be written with a LOT less code.
auto mul(T0, T1)(const T0 m, const T1 v) if(IsMatrix!T0 && IsVector!T1)
{
	MFVector r;
	r.x = m.x.x*v.x + m.y.x*v.y + m.z.x*v.z + m.t.x*v.w;
	r.y = m.x.y*v.x + m.y.y*v.y + m.z.y*v.z + m.t.y*v.w;
	r.z = m.x.z*v.x + m.y.z*v.y + m.z.z*v.z + m.t.z*v.w;
	r.w = m.x.w*v.x + m.y.w*v.y + m.z.w*v.z + m.t.w*v.w;
	return r;
}

auto mul(T0, T1)(const T0 a, const T1 b) if(IsMatrix!T0 && IsMatrix!T1)
{
	static if(is(std.traits.Unqual!(typeof(a)) == MFMatrix) && is(std.traits.Unqual!(typeof(b)) == MFMatrix))
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
	else static if(is(a == MFMatrix) && is(b == MFMatrixx3))
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
	else static if(is(a == MFMatrixx3) && is(b == MFMatrix))
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
	else static if(is(a == MFMatrix) && is(b == Matrix3))
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
	else static if(is(a == Matrix3) && is(b == MFMatrix))
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
	else static if(is(a == MFMatrixx3) && is(b == MFMatrixx3))
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
	else static if(is(a == MFMatrixx3) && is(b == Matrix3))
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
	else static if(is(a == Matrix3) && is(b == MFMatrixx3))
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
	else static if(is(a == Matrix3) && is(b == Matrix3))
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
