#include "Fuji.h"
#include "MFMatrix.h"
#include "MFVector.h"

const MFMatrix MFMatrix::identity = 
{
	{
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	}
};

MFMatrix& MFMatrix::operator=(const MFMatrix& mat)
{
	m[0] =mat.m[0];  m[1] =mat.m[1];  m[2] =mat.m[2];  m[3] =mat.m[3];
	m[4] =mat.m[4];  m[5] =mat.m[5];  m[6] =mat.m[6];  m[7] =mat.m[7];
	m[8] =mat.m[8];  m[9] =mat.m[9];  m[10]=mat.m[10]; m[11]=mat.m[11];
	m[12]=mat.m[12]; m[13]=mat.m[13]; m[14]=mat.m[14]; m[15]=mat.m[15];

	return *this;
}

bool MFMatrix::operator==(const MFMatrix& mat) const
{
	return	m[0] ==mat.m[0]  && m[1] ==mat.m[1]  && m[2] ==mat.m[2]  && m[3] ==mat.m[3]  &&
			m[4] ==mat.m[4]  && m[5] ==mat.m[5]  && m[6] ==mat.m[6]  && m[7] ==mat.m[7]  &&
			m[8] ==mat.m[8]  && m[9] ==mat.m[9]  && m[10]==mat.m[10] && m[11]==mat.m[11] &&
			m[12]==mat.m[12] && m[13]==mat.m[13] && m[14]==mat.m[14] && m[15]==mat.m[15];
}

bool MFMatrix::operator!=(const MFMatrix& mat) const
{
	return	m[0] !=mat.m[0]  || m[1] !=mat.m[1]  || m[2] !=mat.m[2]  || m[3] !=mat.m[3]  ||
			m[4] !=mat.m[4]  || m[5] !=mat.m[5]  || m[6] !=mat.m[6]  || m[7] !=mat.m[7]  ||
			m[8] !=mat.m[8]  || m[9] !=mat.m[9]  || m[10]!=mat.m[10] || m[11]!=mat.m[11] ||
			m[12]!=mat.m[12] || m[13]!=mat.m[13] || m[14]!=mat.m[14] || m[15]!=mat.m[15];
}

MFMatrix& MFMatrix::Rotate(const MFVector &axis, float angle)
{
	MFMatrix mat;
	mat.SetRotation(axis, angle);
	return Multiply(mat, *this);
}

MFMatrix& MFMatrix::RotateQ(const MFQuaternion &q)
{
	MFMatrix mat;
	mat.SetRotationQ(q);
	return Multiply(mat, *this);
}

MFMatrix& MFMatrix::RotateYPR(float yaw, float pitch, float roll)
{
	MFMatrix mat;
	mat.SetRotationYPR(yaw, pitch, roll);
	return Multiply(mat, *this);
}

MFMatrix& MFMatrix::RotateX(float angle)
{
	MFMatrix rot;
	rot.SetRotationX(angle);
	return Multiply3x3(rot);
}

MFMatrix& MFMatrix::RotateY(float angle)
{
	MFMatrix rot;
	rot.SetRotationY(angle);
	return 	Multiply3x3(rot);
}

MFMatrix& MFMatrix::RotateZ(float angle)
{
	MFMatrix rot;
	rot.SetRotationZ(angle);
	return 	Multiply3x3(rot);
}

MFMatrix& MFMatrix::Scale(const MFVector& scale)
{
	MFMatrix mat;
	mat.SetScale(scale);
	return Multiply(mat);
}

MFMatrix& MFMatrix::SetTranslation(const MFVector &trans)
{
	m[1] = m[2] = m[3] = m[4] = m[6] = m[7] = m[8] = m[9] = m[11] = 0.0f;
	m[0] = m[5] = m[10] = m[15] = 1.0f;
	m[12] = trans.x;
	m[13] = trans.y;
	m[14] = trans.z;
	return *this;
}

MFMatrix& MFMatrix::SetRotation(const MFVector &axis, float angle)
{
	float c,s,t;

	// do the trig
	s = MFSin(angle);
	c = MFCos(angle);
	t = 1.0f-c;

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

	m[12] = m[13] = m[14] = m[3]= m[7] = m[11] = 0.0f;
	m[15] = 1.0f;

	return *this;
}

MFMatrix& MFMatrix::SetRotationQ(const MFQuaternion &q)
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

	m[12] = m[13] = m[14] = m[3]= m[7] = m[11] = 0.0f;
	m[15] = 1.0f;

	return *this;
}

/*
MFQuaternion MFMatrix::GetRotationQ() const
{

	Calculate the trace of the matrix T from the equation:

                2     2     2
      T = 4 - 4x  - 4y  - 4z

                 2    2    2
        = 4( 1 -x  - y  - z )

        = mat[0] + mat[5] + mat[10] + 1


    If the trace of the matrix is greater than zero, then
    perform an "instant" calculation.

      S = 0.5 / sqrt(T)

      W = 0.25 / S

      X = ( mat[9] - mat[6] ) * S

      Y = ( mat[2] - mat[8] ) * S

      Z = ( mat[4] - mat[1] ) * S


    If the trace of the matrix is less than or equal to zero
    then identify which major diagonal element has the greatest
    value.

    Depending on this value, calculate the following:

      Column 0:
        S  = sqrt( 1.0 + mr[0] - mr[5] - mr[10] ) * 2;

        Qx = 0.5 / S;
        Qy = (mr[1] + mr[4] ) / S;
        Qz = (mr[2] + mr[8] ) / S;
        Qw = (mr[6] + mr[9] ) / S;

      Column 1:
        S  = sqrt( 1.0 + mr[5] - mr[0] - mr[10] ) * 2;

        Qx = (mr[1] + mr[4] ) / S;
        Qy = 0.5 / S;
        Qz = (mr[6] + mr[9] ) / S;
        Qw = (mr[2] + mr[8] ) / S;

      Column 2:
        S  = sqrt( 1.0 + mr[10] - mr[0] - mr[5] ) * 2;

        Qx = (mr[2] + mr[8] ) / S;
        Qy = (mr[6] + mr[9] ) / S;
        Qz = 0.5 / S;
        Qw = (mr[1] + mr[4] ) / S;

     The quaternion is then defined as:

       Q = | Qx Qy Qz Qw |

	return MFVector::identity;
}
*/

MFMatrix& MFMatrix::SetRotationYPR(float yaw, float pitch, float roll)
{
	float cosy = MFCos(yaw);
	float siny = MFSin(yaw);
	float cosp = MFCos(pitch);
	float sinp = MFSin(pitch);
	float cosr = MFCos(roll);
	float sinr = MFSin(roll);

	m[0] =  cosr * cosy + sinp * sinr * siny;
	m[1] =  cosp * sinr;
	m[2] =  cosy * sinp * sinr - cosr * siny;

	m[4] = -cosy * sinr + cosr * sinp * siny;
	m[5] =  cosp * cosr;
	m[6] =  cosr * cosy * sinp + sinr * siny;

	m[8] =  cosp * siny;
	m[9] = -sinp;
	m[10] =  cosp * cosy;

	m[3] = m[7] = m[11] = m[12] = m[13] = m[14] = 0.0f;
	m[15] = 1.0f;

	return *this;
}

MFMatrix& MFMatrix::SetRotationX(float angle)
{
	m[0] = 1.0f;
	m[1] = 0.0f;
	m[2] = 0.0f;
	m[4] = 0.0f;
	m[5] = MFCos(angle);
	m[6] = MFSin(angle);
	m[8] = 0.0f;
	m[9] = -MFSin(angle);
	m[10] = MFCos(angle);
	m[12] = m[13] = m[14] = 0.0f;
	m[15] = 1.0f;

	return *this;
}

MFMatrix& MFMatrix::SetRotationY(float angle)
{
	m[0] = MFCos(angle);
	m[1] = 0.0f;
	m[2] = -MFSin(angle);
	m[4] = 0.0f;
	m[5] = 1.0f;
	m[6] = 0.0f;
	m[8] = MFSin(angle);
	m[9] = 0.0f;
	m[10] = MFCos(angle);
	m[12] = m[13] = m[14] = 0.0f;
	m[15] = 1.0f;

	return *this;
}

MFMatrix& MFMatrix::SetRotationZ(float angle)
{
	m[0] = MFCos(angle);
	m[1] = MFSin(angle);
	m[2] = 0.0f;
	m[4] = -MFSin(angle);
	m[5] = MFCos(angle);
	m[6] = 0.0f;
	m[8] = 0.0f;
	m[9] = 0.0f;
	m[10] = 1.0f;
	m[12] = m[13] = m[14] = 0.0f;
	m[15] = 1.0f;

	return *this;
}

MFMatrix& MFMatrix::SetScale(const MFVector& scale)
{
	m[0] = scale.x;
	m[1] = m[2] = m[3] = m[4] = 0.0f;
	m[5] = scale.y;
	m[6] = m[7] = m[8] = m[9] = 0.0f;
	m[10] = scale.z;
	m[11] = m[12] = m[13] = m[14] = 0.0f;
	m[15] = 1.0f;

	return *this;
}

MFMatrix& MFMatrix::LookAt(const MFVector& pos, const MFVector& at, const MFVector& up)
{
	MFVector &xAxis = *(MFVector*)&m[0];
	MFVector &yAxis = *(MFVector*)&m[4];
	MFVector &zAxis = *(MFVector*)&m[8];
	MFVector &trans = *(MFVector*)&m[12];

	// calculate forewards
	zAxis.Normalise3(at-pos);

	// calculate right
	xAxis.Cross3(up, zAxis);
	xAxis.Normalise3();

	// calculate up
	yAxis.Cross3(zAxis, xAxis);
	yAxis.Normalise3();

	// calculate translation
	trans = pos;

	return *this;
}

MFMatrix& MFMatrix::Transpose()
{
	register float t;
	t=m[4]; m[4]=m[1]; m[1]=t;
	t=m[8]; m[8]=m[2]; m[2]=t;
	t=m[9]; m[9]=m[6]; m[6]=t;
	t=m[12]; m[12]=m[3]; m[3]=t;
	t=m[13]; m[13]=m[7]; m[7]=t;
	t=m[14]; m[14]=m[11]; m[11]=t;
	return *this;
}

MFMatrix& MFMatrix::Transpose(const MFMatrix &mat)
{
	if(this == &mat)
	{
		return Transpose();
	}

	m[0] = mat.m[0];
	m[1] = mat.m[4];
	m[2] = mat.m[8];
	m[3] = mat.m[12];
	m[4] = mat.m[1];
	m[5] = mat.m[5];
	m[6] = mat.m[9];
	m[7] = mat.m[13];
	m[8] = mat.m[2];
	m[9] = mat.m[6];
	m[10] = mat.m[10];
	m[11] = mat.m[14];
	m[12] = mat.m[3];
	m[13] = mat.m[7];
	m[14] = mat.m[11];
	m[15] = mat.m[15];

	return *this;
}

MFMatrix& MFMatrix::Transpose3x3()
{
	register float t;
	t=m[4]; m[4]=m[1]; m[1]=t;
	t=m[8]; m[8]=m[2]; m[2]=t;
	t=m[9]; m[9]=m[6]; m[6]=t;
	return *this;
}

MFMatrix& MFMatrix::Transpose3x3(const MFMatrix &mat)
{
	if(this == &mat)
		return Transpose3x3();

	m[0] = mat.m[0];
	m[1] = mat.m[4];
	m[2] = mat.m[8];
	m[3] = mat.m[3];
	m[4] = mat.m[1];
	m[5] = mat.m[5];
	m[6] = mat.m[9];
	m[7] = mat.m[7];
	m[8] = mat.m[2];
	m[9] = mat.m[6];
	m[10] = mat.m[10];
	m[11] = mat.m[11];
	m[12] = mat.m[12];
	m[13] = mat.m[13];
	m[14] = mat.m[14];
	m[15] = mat.m[15];

	return *this;
}

MFMatrix& MFMatrix::Multiply(const MFMatrix &mat)
{
	MFMatrix t;

	t.m[0] = m[0]*mat.m[0] + m[1]*mat.m[4] + m[2]*mat.m[8];
	t.m[1] = m[0]*mat.m[1] + m[1]*mat.m[5] + m[2]*mat.m[9];
	t.m[2] = m[0]*mat.m[2] + m[1]*mat.m[6] + m[2]*mat.m[10];
	t.m[4] = m[4]*mat.m[0] + m[5]*mat.m[4] + m[6]*mat.m[8];
	t.m[5] = m[4]*mat.m[1] + m[5]*mat.m[5] + m[6]*mat.m[9];
	t.m[6] = m[4]*mat.m[2] + m[5]*mat.m[6] + m[6]*mat.m[10];
	t.m[8] = m[8]*mat.m[0] + m[9]*mat.m[4] + m[10]*mat.m[8];
	t.m[9] = m[8]*mat.m[1] + m[9]*mat.m[5] + m[10]*mat.m[9];
	t.m[10] = m[8]*mat.m[2] + m[9]*mat.m[6] + m[10]*mat.m[10];
	t.m[12] = m[12]*mat.m[0] + m[13]*mat.m[4] + m[14]*mat.m[8] + mat.m[12];
	t.m[13] = m[12]*mat.m[1] + m[13]*mat.m[5] + m[14]*mat.m[9] + mat.m[13];
	t.m[14] = m[12]*mat.m[2] + m[13]*mat.m[6] + m[14]*mat.m[10] + mat.m[14];

	m[0] = t.m[0]; m[1] = t.m[1]; m[2] = t.m[2];
	m[4] = t.m[4]; m[5] = t.m[5]; m[6] = t.m[6];
	m[8] = t.m[8]; m[9] = t.m[9]; m[10] = t.m[10];
	m[12] = t.m[12]; m[13] = t.m[13]; m[14] = t.m[14];

	return *this;
}

MFMatrix& MFMatrix::Multiply(const MFMatrix &mat1, const MFMatrix &mat2)
{
	MFMatrix t;

	t.m[0] = mat1.m[0]*mat2.m[0] + mat1.m[1]*mat2.m[4] + mat1.m[2]*mat2.m[8];
	t.m[1] = mat1.m[0]*mat2.m[1] + mat1.m[1]*mat2.m[5] + mat1.m[2]*mat2.m[9];
	t.m[2] = mat1.m[0]*mat2.m[2] + mat1.m[1]*mat2.m[6] + mat1.m[2]*mat2.m[10];
	t.m[4] = mat1.m[4]*mat2.m[0] + mat1.m[5]*mat2.m[4] + mat1.m[6]*mat2.m[8];
	t.m[5] = mat1.m[4]*mat2.m[1] + mat1.m[5]*mat2.m[5] + mat1.m[6]*mat2.m[9];
	t.m[6] = mat1.m[4]*mat2.m[2] + mat1.m[5]*mat2.m[6] + mat1.m[6]*mat2.m[10];
	t.m[8] = mat1.m[8]*mat2.m[0] + mat1.m[9]*mat2.m[4] + mat1.m[10]*mat2.m[8];
	t.m[9] = mat1.m[8]*mat2.m[1] + mat1.m[9]*mat2.m[5] + mat1.m[10]*mat2.m[9];
	t.m[10] = mat1.m[8]*mat2.m[2] + mat1.m[9]*mat2.m[6] + mat1.m[10]*mat2.m[10];
	t.m[12] = mat1.m[12]*mat2.m[0] + mat1.m[13]*mat2.m[4] + mat1.m[14]*mat2.m[8] + mat2.m[12];
	t.m[13] = mat1.m[12]*mat2.m[1] + mat1.m[13]*mat2.m[5] + mat1.m[14]*mat2.m[9] + mat2.m[13];
	t.m[14] = mat1.m[12]*mat2.m[2] + mat1.m[13]*mat2.m[6] + mat1.m[14]*mat2.m[10] + mat2.m[14];

	m[0] = t.m[0]; m[1] = t.m[1]; m[2] = t.m[2];
	m[4] = t.m[4]; m[5] = t.m[5]; m[6] = t.m[6];
	m[8] = t.m[8]; m[9] = t.m[9]; m[10] = t.m[10];
	m[12] = t.m[12]; m[13] = t.m[13]; m[14] = t.m[14];

	return *this;
}

MFMatrix& MFMatrix::Multiply4x4(const MFMatrix &mat)
{
	MFMatrix t;

	t.m[0] = m[0]*mat.m[0] + m[1]*mat.m[4] + m[2]*mat.m[8] + m[3]*mat.m[12];
	t.m[1] = m[0]*mat.m[1] + m[1]*mat.m[5] + m[2]*mat.m[9] + m[3]*mat.m[13];
	t.m[2] = m[0]*mat.m[2] + m[1]*mat.m[6] + m[2]*mat.m[10] + m[3]*mat.m[14];
	t.m[3] = m[0]*mat.m[3] + m[1]*mat.m[7] + m[2]*mat.m[11] + m[3]*mat.m[15];
	t.m[4] = m[4]*mat.m[0] + m[5]*mat.m[4] + m[6]*mat.m[8] + m[7]*mat.m[12];
	t.m[5] = m[4]*mat.m[1] + m[5]*mat.m[5] + m[6]*mat.m[9] + m[7]*mat.m[13];
	t.m[6] = m[4]*mat.m[2] + m[5]*mat.m[6] + m[6]*mat.m[10] + m[7]*mat.m[14];
	t.m[7] = m[4]*mat.m[3] + m[5]*mat.m[7] + m[6]*mat.m[11] + m[7]*mat.m[15];
	t.m[8] = m[8]*mat.m[0] + m[9]*mat.m[4] + m[10]*mat.m[8] + m[11]*mat.m[12];
	t.m[9] = m[8]*mat.m[1] + m[9]*mat.m[5] + m[10]*mat.m[9] + m[11]*mat.m[13];
	t.m[10] = m[8]*mat.m[2] + m[9]*mat.m[6] + m[10]*mat.m[10] + m[11]*mat.m[14];
	t.m[11] = m[8]*mat.m[3] + m[9]*mat.m[7] + m[10]*mat.m[11] + m[11]*mat.m[15];
	t.m[12] = m[12]*mat.m[0] + m[13]*mat.m[4] + m[14]*mat.m[8] + m[15]*mat.m[12];
	t.m[13] = m[12]*mat.m[1] + m[13]*mat.m[5] + m[14]*mat.m[9] + m[15]*mat.m[13];
	t.m[14] = m[12]*mat.m[2] + m[13]*mat.m[6] + m[14]*mat.m[10] + m[15]*mat.m[14];
	t.m[15] = m[12]*mat.m[3] + m[13]*mat.m[7] + m[14]*mat.m[11] + m[15]*mat.m[15];

	m[0] = t.m[0]; m[1] = t.m[1]; m[2] = t.m[2]; m[3] = t.m[3];
	m[4] = t.m[4]; m[5] = t.m[5]; m[6] = t.m[6]; m[7] = t.m[7];
	m[8] = t.m[8]; m[9] = t.m[9]; m[10] = t.m[10]; m[11] = t.m[11];
	m[12] = t.m[12]; m[13] = t.m[13]; m[14] = t.m[14]; m[15] = t.m[15];

	return *this;
}

MFMatrix& MFMatrix::Multiply4x4(const MFMatrix &mat1, const MFMatrix &mat2)
{
	MFMatrix t;

	t.m[0] = mat1.m[0]*mat2.m[0] + mat1.m[1]*mat2.m[4] + mat1.m[2]*mat2.m[8] + mat1.m[3]*mat2.m[12];
	t.m[1] = mat1.m[0]*mat2.m[1] + mat1.m[1]*mat2.m[5] + mat1.m[2]*mat2.m[9] + mat1.m[3]*mat2.m[13];
	t.m[2] = mat1.m[0]*mat2.m[2] + mat1.m[1]*mat2.m[6] + mat1.m[2]*mat2.m[10] + mat1.m[3]*mat2.m[14];
	t.m[3] = mat1.m[0]*mat2.m[3] + mat1.m[1]*mat2.m[7] + mat1.m[2]*mat2.m[11] + mat1.m[3]*mat2.m[15];
	t.m[4] = mat1.m[4]*mat2.m[0] + mat1.m[5]*mat2.m[4] + mat1.m[6]*mat2.m[8] + mat1.m[7]*mat2.m[12];
	t.m[5] = mat1.m[4]*mat2.m[1] + mat1.m[5]*mat2.m[5] + mat1.m[6]*mat2.m[9] + mat1.m[7]*mat2.m[13];
	t.m[6] = mat1.m[4]*mat2.m[2] + mat1.m[5]*mat2.m[6] + mat1.m[6]*mat2.m[10] + mat1.m[7]*mat2.m[14];
	t.m[7] = mat1.m[4]*mat2.m[3] + mat1.m[5]*mat2.m[7] + mat1.m[6]*mat2.m[11] + mat1.m[7]*mat2.m[15];
	t.m[8] = mat1.m[8]*mat2.m[0] + mat1.m[9]*mat2.m[4] + mat1.m[10]*mat2.m[8] + mat1.m[11]*mat2.m[12];
	t.m[9] = mat1.m[8]*mat2.m[1] + mat1.m[9]*mat2.m[5] + mat1.m[10]*mat2.m[9] + mat1.m[11]*mat2.m[13];
	t.m[10] = mat1.m[8]*mat2.m[2] + mat1.m[9]*mat2.m[6] + mat1.m[10]*mat2.m[10] + mat1.m[11]*mat2.m[14];
	t.m[11] = mat1.m[8]*mat2.m[3] + mat1.m[9]*mat2.m[7] + mat1.m[10]*mat2.m[11] + mat1.m[11]*mat2.m[15];
	t.m[12] = mat1.m[12]*mat2.m[0] + mat1.m[13]*mat2.m[4] + mat1.m[14]*mat2.m[8] + mat1.m[15]*mat2.m[12];
	t.m[13] = mat1.m[12]*mat2.m[1] + mat1.m[13]*mat2.m[5] + mat1.m[14]*mat2.m[9] + mat1.m[15]*mat2.m[13];
	t.m[14] = mat1.m[12]*mat2.m[2] + mat1.m[13]*mat2.m[6] + mat1.m[14]*mat2.m[10] + mat1.m[15]*mat2.m[14];
	t.m[15] = mat1.m[12]*mat2.m[3] + mat1.m[13]*mat2.m[7] + mat1.m[14]*mat2.m[11] + mat1.m[15]*mat2.m[15];

	m[0] = t.m[0]; m[1] = t.m[1]; m[2] = t.m[2]; m[3] = t.m[3];
	m[4] = t.m[4]; m[5] = t.m[5]; m[6] = t.m[6]; m[7] = t.m[7];
	m[8] = t.m[8]; m[9] = t.m[9]; m[10] = t.m[10]; m[11] = t.m[11];
	m[12] = t.m[12]; m[13] = t.m[13]; m[14] = t.m[14]; m[15] = t.m[15];

	return *this;
}

MFMatrix& MFMatrix::Multiply3x3(const MFMatrix &mat)
{
	MFMatrix t;

	t.m[0] = m[0]*mat.m[0] + m[1]*mat.m[4] + m[2]*mat.m[8];
	t.m[1] = m[0]*mat.m[1] + m[1]*mat.m[5] + m[2]*mat.m[9];
	t.m[2] = m[0]*mat.m[2] + m[1]*mat.m[6] + m[2]*mat.m[10];
	t.m[4] = m[4]*mat.m[0] + m[5]*mat.m[4] + m[6]*mat.m[8];
	t.m[5] = m[4]*mat.m[1] + m[5]*mat.m[5] + m[6]*mat.m[9];
	t.m[6] = m[4]*mat.m[2] + m[5]*mat.m[6] + m[6]*mat.m[10];
	t.m[8] = m[8]*mat.m[0] + m[9]*mat.m[4] + m[10]*mat.m[8];
	t.m[9] = m[8]*mat.m[1] + m[9]*mat.m[5] + m[10]*mat.m[9];
	t.m[10] = m[8]*mat.m[2] + m[9]*mat.m[6] + m[10]*mat.m[10];

	m[0] = t.m[0]; m[1] = t.m[1]; m[2] = t.m[2];
	m[4] = t.m[4]; m[5] = t.m[5]; m[6] = t.m[6];
	m[8] = t.m[8]; m[9] = t.m[9]; m[10] = t.m[10];

	return *this;
}

MFMatrix& MFMatrix::Multiply3x3(const MFMatrix &mat1, const MFMatrix &mat2)
{
	MFMatrix t;

	t.m[0] = mat1.m[0]*mat2.m[0] + mat1.m[1]*mat2.m[4] + mat1.m[2]*mat2.m[8];
	t.m[1] = mat1.m[0]*mat2.m[1] + mat1.m[1]*mat2.m[5] + mat1.m[2]*mat2.m[9];
	t.m[2] = mat1.m[0]*mat2.m[2] + mat1.m[1]*mat2.m[6] + mat1.m[2]*mat2.m[10];
	t.m[4] = mat1.m[4]*mat2.m[0] + mat1.m[5]*mat2.m[4] + mat1.m[6]*mat2.m[8];
	t.m[5] = mat1.m[4]*mat2.m[1] + mat1.m[5]*mat2.m[5] + mat1.m[6]*mat2.m[9];
	t.m[6] = mat1.m[4]*mat2.m[2] + mat1.m[5]*mat2.m[6] + mat1.m[6]*mat2.m[10];
	t.m[8] = mat1.m[8]*mat2.m[0] + mat1.m[9]*mat2.m[4] + mat1.m[10]*mat2.m[8];
	t.m[9] = mat1.m[8]*mat2.m[1] + mat1.m[9]*mat2.m[5] + mat1.m[10]*mat2.m[9];
	t.m[10] = mat1.m[8]*mat2.m[2] + mat1.m[9]*mat2.m[6] + mat1.m[10]*mat2.m[10];

	m[0] = t.m[0]; m[1] = t.m[1]; m[2] = t.m[2];
	m[4] = t.m[4]; m[5] = t.m[5]; m[6] = t.m[6];
	m[8] = t.m[8]; m[9] = t.m[9]; m[10] = t.m[10];

	return *this;
}

// matrix inverse shamelessly taken from some math book...
#define ACCUMULATE    \
	if(temp >= 0.0)   \
		pos += temp;  \
	else              \
		neg += temp;

#define PRECISION_LIMIT (1.0e-10)

MFMatrix& MFMatrix::Inverse(const MFMatrix &mat)
{
	MFMatrix out;
	register float det_1;
	float pos, neg, temp;

	//	* Calculate the determinant of submatrix A and determine if the
	//	* the matrix is singular as limited by the double precision
	//	* floating-point data representation.
	pos = neg = 0.0;
	temp =  mat.m[0] * mat.m[5] * mat.m[10];
	ACCUMULATE
	temp =  mat.m[1] * mat.m[6] * mat.m[8];
	ACCUMULATE
	temp =  mat.m[2] * mat.m[4] * mat.m[9];
	ACCUMULATE
	temp = -mat.m[2] * mat.m[5] * mat.m[8];
	ACCUMULATE
	temp = -mat.m[1] * mat.m[4] * mat.m[10];
	ACCUMULATE
	temp = -mat.m[0] * mat.m[6] * mat.m[9];
	ACCUMULATE
	det_1 = pos + neg;

	// Is the submatrix A singular?
	if ((det_1 == 0.0f) || (fabsf(det_1 / (pos - neg)) < PRECISION_LIMIT))
	{
		// MFMatrix M has no inverse
		MFDebug_Warn(3, "MFMatrix::Inverse: Singular matrix (Matrix has no inverse)...\n");
		return *this;
	}

	// Calculate inverse(A) = adj(A) / det(A)
	det_1 = 1.0f / det_1;
	out.m[0] =  (mat.m[5]*mat.m[10] - mat.m[6]*mat.m[9]) * det_1;
	out.m[4] = -(mat.m[4]*mat.m[10] - mat.m[6]*mat.m[8]) * det_1;
	out.m[8] =  (mat.m[4]*mat.m[9] - mat.m[5]*mat.m[8]) * det_1;
	out.m[1] = -(mat.m[1]*mat.m[10] - mat.m[2]*mat.m[9]) * det_1;
	out.m[5] =  (mat.m[0]*mat.m[10] - mat.m[2]*mat.m[8]) * det_1;
	out.m[9] = -(mat.m[0]*mat.m[9] - mat.m[1]*mat.m[8]) * det_1;
	out.m[2] =  (mat.m[1]*mat.m[6] - mat.m[2]*mat.m[5]) * det_1;
	out.m[6] = -(mat.m[0]*mat.m[6] - mat.m[2]*mat.m[4]) * det_1;
	out.m[10] =  (mat.m[0]*mat.m[5] - mat.m[1]*mat.m[4]) * det_1;

	// Calculate -C * inverse(A)
	out.m[12] = -(mat.m[12]*out.m[0] + mat.m[13]*out.m[4] + mat.m[14]*out.m[8]);
	out.m[13] = -(mat.m[12]*out.m[1] + mat.m[13]*out.m[5] + mat.m[14]*out.m[9]);
	out.m[14] = -(mat.m[12]*out.m[2] + mat.m[13]*out.m[6] + mat.m[14]*out.m[10]);

	// Fill in last column
	out.m[3] = out.m[7] = out.m[11] = 0.0f;
	out.m[15] = 1.0f;

	return *this = out;
}

MFMatrix& MFMatrix::Tween(const MFMatrix& start, const MFMatrix& end, float time)
{
	MFMatrix t = start;

	*(MFVector*)&m[0] = ((MFVector*)&t.m[0])->Lerp(*(MFVector*)&end.m[0], time);
	*(MFVector*)&m[4] = ((MFVector*)&t.m[4])->Lerp(*(MFVector*)&end.m[4], time);
	*(MFVector*)&m[8] = ((MFVector*)&t.m[8])->Lerp(*(MFVector*)&end.m[8], time);
	*(MFVector*)&m[12] = ((MFVector*)&t.m[12])->Lerp(*(MFVector*)&end.m[12], time);

	return *this;
}

MFQuaternion MFMatrix::CalculateQuaternion()
{
	MFQuaternion t;
	float T = m[0] + m[5] + m[10];
	if(T>0.0f)
	{
//		If the trace of the matrix is greater than zero, then
//		perform an "instant" calculation.
		float S=0.5f/MFSqrt(T+1);
		t.w = 0.25f/S;
		t.x = (m[9] - m[6])*S;
		t.y = (m[2] - m[8])*S;
		t.z = (m[4] - m[1])*S;
	}
	else
	{
//		If the trace of the matrix is less than or equal to zero
//		then identify which major diagonal element has the greatest
//		value.
//		Depending on this value, calculate the following:
		if ((m[0] > m[5]) && (m[0] > m[10]))
		{
			float S = MFSqrt(1.0f + m[0] - m[5] - m[10]) * 2.0f;
			t.x = 0.25f * S;
			t.y = (m[1] + m[4] ) / S;
			t.z = (m[2] + m[8] ) / S;
			t.w = (m[6] - m[9] ) / S;
		}
		else if (m[5] > m[10])
		{
			float S = MFSqrt(1.0f + m[5] - m[0] - m[10]) * 2.0f;
			t.x = (m[1] + m[4]) / S;
			t.y = 0.25f * S;
			t.z = (m[6] + m[9]) / S;
			t.w = (m[2] - m[8]) / S;
		}
		else
		{ 
			float S = MFSqrt(1.0f + m[10] - m[0] - m[5] ) * 2.0f;
			t.x = (m[2] + m[8]) / S;
			t.y = (m[6] + m[9]) / S;
			t.z = 0.25f * S;
			t.w = (m[1] - m[4]) / S;
		}
	}

	return t;
}

/*
	Calculate the trace of the matrix T from the equation:

                2     2     2
      T = 4 - 4x  - 4y  - 4z

                 2    2    2
        = 4( 1 -x  - y  - z )

        = mat[0] + mat[5] + mat[10] + 1


    If the trace of the matrix is greater than zero, then
    perform an "instant" calculation.

      S = 0.5 / sqrt(T)

      W = 0.25 / S

      X = ( mat[9] - mat[6] ) * S

      Y = ( mat[2] - mat[8] ) * S

      Z = ( mat[4] - mat[1] ) * S


    If the trace of the matrix is less than or equal to zero
    then identify which major diagonal element has the greatest
    value.

    Depending on this value, calculate the following:

      Column 0:
        S  = sqrt( 1.0 + mr[0] - mr[5] - mr[10] ) * 2;

        Qx = 0.5 / S;
        Qy = (mr[1] + mr[4] ) / S;
        Qz = (mr[2] + mr[8] ) / S;
        Qw = (mr[6] + mr[9] ) / S;

      Column 1:
        S  = sqrt( 1.0 + mr[5] - mr[0] - mr[10] ) * 2;

        Qx = (mr[1] + mr[4] ) / S;
        Qy = 0.5 / S;
        Qz = (mr[6] + mr[9] ) / S;
        Qw = (mr[2] + mr[8] ) / S;

      Column 2:
        S  = sqrt( 1.0 + mr[10] - mr[0] - mr[5] ) * 2;

        Qx = (mr[2] + mr[8] ) / S;
        Qy = (mr[6] + mr[9] ) / S;
        Qz = 0.5 / S;
        Qw = (mr[1] + mr[4] ) / S;

     The quaternion is then defined as:

       Q = | Qx Qy Qz Qw |
	   */

