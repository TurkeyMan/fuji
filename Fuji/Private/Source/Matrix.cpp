#include "Common.h"
#include "Matrix.h"
#include "Vector3.h"
#include "Vector4.h"

const Matrix Matrix::identity = 
{
	{
		{ 1.0f, 0.0f, 0.0f, 0.0f },
		{ 0.0f, 1.0f, 0.0f, 0.0f },
		{ 0.0f, 0.0f, 1.0f, 0.0f },
		{ 0.0f, 0.0f, 0.0f, 1.0f }
	}
};

Matrix& Matrix::operator=(const Matrix& mat)
{
	m[0][0]=mat.m[0][0]; m[0][1]=mat.m[0][1]; m[0][2]=mat.m[0][2]; m[0][3]=mat.m[0][3];
	m[1][0]=mat.m[1][0]; m[1][1]=mat.m[1][1]; m[1][2]=mat.m[1][2]; m[1][3]=mat.m[1][3];
	m[2][0]=mat.m[2][0]; m[2][1]=mat.m[2][1]; m[2][2]=mat.m[2][2]; m[2][3]=mat.m[2][3];
	m[3][0]=mat.m[3][0]; m[3][1]=mat.m[3][1]; m[3][2]=mat.m[3][2]; m[3][3]=mat.m[3][3];

	return *this;
}

bool Matrix::operator==(const Matrix& mat) const
{
	return	m[0][0]==mat.m[0][0] && m[0][1]==mat.m[0][1] && m[0][2]==mat.m[0][2] && m[0][3]==mat.m[0][3] &&
			m[1][0]==mat.m[1][0] && m[1][1]==mat.m[1][1] && m[1][2]==mat.m[1][2] && m[1][3]==mat.m[1][3] &&
			m[2][0]==mat.m[2][0] && m[2][1]==mat.m[2][1] && m[2][2]==mat.m[2][2] && m[2][3]==mat.m[2][3] &&
			m[3][0]==mat.m[3][0] && m[3][1]==mat.m[3][1] && m[3][2]==mat.m[3][2] && m[3][3]==mat.m[3][3];
}

bool Matrix::operator!=(const Matrix& mat) const
{
	return	m[0][0]!=mat.m[0][0] || m[0][1]!=mat.m[0][1] || m[0][2]!=mat.m[0][2] || m[0][3]!=mat.m[0][3] ||
			m[1][0]!=mat.m[1][0] || m[1][1]!=mat.m[1][1] || m[1][2]!=mat.m[1][2] || m[1][3]!=mat.m[1][3] ||
			m[2][0]!=mat.m[2][0] || m[2][1]!=mat.m[2][1] || m[2][2]!=mat.m[2][2] || m[2][3]!=mat.m[2][3] ||
			m[3][0]!=mat.m[3][0] || m[3][1]!=mat.m[3][1] || m[3][2]!=mat.m[3][2] || m[3][3]!=mat.m[3][3];
}

Matrix& Matrix::SetIdentity()
{
	*this = Matrix::identity;
	return *this;
}

Matrix& Matrix::Translate(const Vector3 &trans)
{
	m[3][0] += trans.x;
	m[3][1] += trans.y;
	m[3][2] += trans.z;
	return *this;
}

Matrix& Matrix::Rotate(const Vector3 &axis, float angle)
{
	DBGASSERT(false, "Not Written...");
	return *this;
}

Matrix& Matrix::RotateQ(const Vector3 &axis, float angle)
{
	DBGASSERT(false, "Not Written...");
	return *this;
}

Matrix& Matrix::RotateYPR(float yaw, float pitch, float roll)
{
	Matrix mat;
	mat.SetRotationYPR(yaw, pitch, roll);
	return Multiply(mat, *this);
}

Matrix& Matrix::RotateX(float angle)
{
	Matrix rot;
	rot.SetRotationX(angle);
	return Multiply3x3(rot);
}

Matrix& Matrix::RotateY(float angle)
{
	Matrix rot;
	rot.SetRotationY(angle);
	return 	Multiply3x3(rot);
}

Matrix& Matrix::RotateZ(float angle)
{
	Matrix rot;
	rot.SetRotationZ(angle);
	return 	Multiply3x3(rot);
}

Matrix& Matrix::Scale(const Vector3& scale)
{
	Matrix mat;
	mat.SetScale(scale);
	return Multiply(mat);
}

Matrix& Matrix::SetTranslation(const Vector3 &trans)
{
	m[0][1] = m[0][2] = m[0][3] = m[1][0] = m[1][2] = m[1][3] = m[2][0] = m[2][1] = m[2][3] = 0.0f;
	m[0][0] = m[1][1] = m[2][2] = m[3][3] = 1.0f;
	m[3][0] = trans.x;
	m[3][1] = trans.y;
	m[3][2] = trans.z;
	return *this;
}

Matrix& Matrix::SetRotation(const Vector3 &axis, float angle)
{
	float c,s,t;

	// do the trig
	s = MFSin(angle);
	c = MFCos(angle);
	t = 1.0f-c;

	// build the rotation matrix
	m[0][0] = t*axis.x*axis.x + c;
	m[1][0] = t*axis.x*axis.y - s*axis.z;
	m[2][0] = t*axis.x*axis.z + s*axis.y;

	m[0][1] = t*axis.x*axis.y + s*axis.z;
	m[1][1] = t*axis.y*axis.y + c;
	m[2][1] = t*axis.y*axis.z - s*axis.x;

	m[0][2] = t*axis.x*axis.z - s*axis.y;
	m[1][2] = t*axis.y*axis.z + s*axis.x;
	m[2][2] = t*axis.z*axis.z + c;

	m[3][0] = m[3][1] = m[3][2] = m[0][3]= m[1][3] = m[2][3] = 0.0f;
	m[3][3] = 1.0f;

	return *this;
}

Matrix& Matrix::SetRotationQ(const Vector4 &q)
{
	DBGASSERT(false, "Not Written...");
	return *this;
}

Matrix& Matrix::SetRotationYPR(float yaw, float pitch, float roll)
{
	float cosy = MFCos(yaw);
	float siny = MFSin(yaw);
	float cosp = MFCos(pitch);
	float sinp = MFSin(pitch);
	float cosr = MFCos(roll);
	float sinr = MFSin(roll);

	m[0][0] =  cosr * cosy + sinp * sinr * siny;
	m[0][1] =  cosp * sinr;
	m[0][2] =  cosy * sinp * sinr - cosr * siny;
	m[0][3] =  0.0f;

	m[1][0] = -cosy * sinr + cosr * sinp * siny;
	m[1][1] =  cosp * cosr;
	m[1][2] =  cosr * cosy * sinp + sinr * siny;
	m[1][3] =  0.0f;

	m[2][0] =  cosp * siny;
	m[2][1] = -sinp;
	m[2][2] =  cosp * cosy;
	m[2][3] =  0.0f;

	m[3][0] = m[3][1] = m[3][2] = 0.0f;
	m[3][3] = 1.0f;

	return *this;
}

Matrix& Matrix::SetRotationX(float angle)
{
	m[0][0] = 1.0f;
	m[0][1] = 0.0f;
	m[0][2] = 0.0f;
	m[1][0] = 0.0f;
	m[1][1] = MFCos(angle);
	m[1][2] = MFSin(angle);
	m[2][0] = 0.0f;
	m[2][1] = -MFSin(angle);
	m[2][2] = MFCos(angle);
	m[3][0] = m[3][1] = m[3][2] = 0.0f;
	m[3][3] = 1.0f;

	return *this;
}

Matrix& Matrix::SetRotationY(float angle)
{
	m[0][0] = MFCos(angle);
	m[0][1] = 0.0f;
	m[0][2] = -MFSin(angle);
	m[1][0] = 0.0f;
	m[1][1] = 1.0f;
	m[1][2] = 0.0f;
	m[2][0] = MFSin(angle);
	m[2][1] = 0.0f;
	m[2][2] = MFCos(angle);
	m[3][0] = m[3][1] = m[3][2] = 0.0f;
	m[3][3] = 1.0f;

	return *this;
}

Matrix& Matrix::SetRotationZ(float angle)
{
	m[0][0] = MFCos(angle);
	m[0][1] = MFSin(angle);
	m[0][2] = 0.0f;
	m[1][0] = -MFSin(angle);
	m[1][1] = MFCos(angle);
	m[1][2] = 0.0f;
	m[2][0] = 0.0f;
	m[2][1] = 0.0f;
	m[2][2] = 1.0f;
	m[3][0] = m[3][1] = m[3][2] = 0.0f;
	m[3][3] = 1.0f;

	return *this;
}

Matrix& Matrix::SetScale(const Vector3& scale)
{
	m[0][0] = scale.x;
	m[0][1] = m[0][2] = m[0][3] = m[1][0] = 0.0f;
	m[1][1] = scale.y;
	m[1][2] = m[1][3] = m[2][0] = m[2][1] = 0.0f;
	m[2][2] = scale.z;
	m[2][3] = m[3][0] = m[3][1] = m[3][2] = 0.0f;
	m[3][3] = 1.0f;

	return *this;
}

Matrix& Matrix::LookAt(const Vector3& pos, const Vector3& at, const Vector3& up)
{
	Vector3 zAxis, xAxis, yAxis, trans;

	// calculate forewards
	zAxis = at-pos;
	zAxis.Normalise();

	// calculate right
	xAxis = up.Cross(zAxis);
	xAxis.Normalise();

	// calculate up
	yAxis = zAxis.Cross(xAxis);
	yAxis.Normalise();

	// calculate translation
//	trans.x = -pos.Dot(xAxis);
//	trans.y = -pos.Dot(yAxis);
//	trans.z = -pos.Dot(zAxis);
	trans = pos;

	SetXAxis(xAxis);
	SetYAxis(yAxis);
	SetZAxis(zAxis);
	SetTrans(trans);

	return *this;
}

Matrix& Matrix::Transpose()
{
	register float t;
	t=m[1][0]; m[1][0]=m[0][1]; m[0][1]=t;
	t=m[2][0]; m[2][0]=m[0][2]; m[0][2]=t;
	t=m[2][1]; m[2][1]=m[1][2]; m[1][2]=t;
	t=m[3][0]; m[3][0]=m[0][3]; m[0][3]=t;
	t=m[3][1]; m[3][1]=m[1][3]; m[1][3]=t;
	t=m[3][2]; m[3][2]=m[2][3]; m[2][3]=t;
	return *this;
}

Matrix& Matrix::Transpose(const Matrix &mat)
{
	if(this == &mat)
	{
		return Transpose();
	}

	m[0][0] = mat.m[0][0];
	m[0][1] = mat.m[1][0];
	m[0][2] = mat.m[2][0];
	m[0][3] = mat.m[3][0];
	m[1][0] = mat.m[0][1];
	m[1][1] = mat.m[1][1];
	m[1][2] = mat.m[2][1];
	m[1][3] = mat.m[3][1];
	m[2][0] = mat.m[0][2];
	m[2][1] = mat.m[1][2];
	m[2][2] = mat.m[2][2];
	m[2][3] = mat.m[3][2];
	m[3][0] = mat.m[0][3];
	m[3][1] = mat.m[1][3];
	m[3][2] = mat.m[2][3];
	m[3][3] = mat.m[3][3];

	return *this;
}

Matrix& Matrix::Transpose3x3()
{
	register float t;
	t=m[1][0]; m[1][0]=m[0][1]; m[0][1]=t;
	t=m[2][0]; m[2][0]=m[0][2]; m[0][2]=t;
	t=m[2][1]; m[2][1]=m[1][2]; m[1][2]=t;
	return *this;
}

Matrix& Matrix::Transpose3x3(const Matrix &mat)
{
	if(this == &mat)
		return Transpose3x3();

	m[0][0] = mat.m[0][0];
	m[0][1] = mat.m[1][0];
	m[0][2] = mat.m[2][0];
	m[0][3] = mat.m[0][3];
	m[1][0] = mat.m[0][1];
	m[1][1] = mat.m[1][1];
	m[1][2] = mat.m[2][1];
	m[1][3] = mat.m[1][3];
	m[2][0] = mat.m[0][2];
	m[2][1] = mat.m[1][2];
	m[2][2] = mat.m[2][2];
	m[2][3] = mat.m[2][3];
	m[3][0] = mat.m[3][0];
	m[3][1] = mat.m[3][1];
	m[3][2] = mat.m[3][2];
	m[3][3] = mat.m[3][3];

	return *this;
}

Matrix& Matrix::Multiply(const Matrix &mat)
{
	Matrix t;

	t.m[0][0] = m[0][0]*mat.m[0][0] + m[0][1]*mat.m[1][0] + m[0][2]*mat.m[2][0];
	t.m[0][1] = m[0][0]*mat.m[0][1] + m[0][1]*mat.m[1][1] + m[0][2]*mat.m[2][1];
	t.m[0][2] = m[0][0]*mat.m[0][2] + m[0][1]*mat.m[1][2] + m[0][2]*mat.m[2][2];
	t.m[1][0] = m[1][0]*mat.m[0][0] + m[1][1]*mat.m[1][0] + m[1][2]*mat.m[2][0];
	t.m[1][1] = m[1][0]*mat.m[0][1] + m[1][1]*mat.m[1][1] + m[1][2]*mat.m[2][1];
	t.m[1][2] = m[1][0]*mat.m[0][2] + m[1][1]*mat.m[1][2] + m[1][2]*mat.m[2][2];
	t.m[2][0] = m[2][0]*mat.m[0][0] + m[2][1]*mat.m[1][0] + m[2][2]*mat.m[2][0];
	t.m[2][1] = m[2][0]*mat.m[0][1] + m[2][1]*mat.m[1][1] + m[2][2]*mat.m[2][1];
	t.m[2][2] = m[2][0]*mat.m[0][2] + m[2][1]*mat.m[1][2] + m[2][2]*mat.m[2][2];
	t.m[3][0] = m[3][0]*mat.m[0][0] + m[3][1]*mat.m[1][0] + m[3][2]*mat.m[2][0] + mat.m[3][0];
	t.m[3][1] = m[3][0]*mat.m[0][1] + m[3][1]*mat.m[1][1] + m[3][2]*mat.m[2][1] + mat.m[3][1];
	t.m[3][2] = m[3][0]*mat.m[0][2] + m[3][1]*mat.m[1][2] + m[3][2]*mat.m[2][2] + mat.m[3][2];

	m[0][0] = t.m[0][0]; m[0][1] = t.m[0][1]; m[0][2] = t.m[0][2];
	m[1][0] = t.m[1][0]; m[1][1] = t.m[1][1]; m[1][2] = t.m[1][2];
	m[2][0] = t.m[2][0]; m[2][1] = t.m[2][1]; m[2][2] = t.m[2][2];
	m[3][0] = t.m[3][0]; m[3][1] = t.m[3][1]; m[3][2] = t.m[3][2];

	return *this;
}

Matrix& Matrix::Multiply(const Matrix &mat1, const Matrix &mat2)
{
	Matrix t;

	t.m[0][0] = mat1.m[0][0]*mat2.m[0][0] + mat1.m[0][1]*mat2.m[1][0] + mat1.m[0][2]*mat2.m[2][0];
	t.m[0][1] = mat1.m[0][0]*mat2.m[0][1] + mat1.m[0][1]*mat2.m[1][1] + mat1.m[0][2]*mat2.m[2][1];
	t.m[0][2] = mat1.m[0][0]*mat2.m[0][2] + mat1.m[0][1]*mat2.m[1][2] + mat1.m[0][2]*mat2.m[2][2];
	t.m[1][0] = mat1.m[1][0]*mat2.m[0][0] + mat1.m[1][1]*mat2.m[1][0] + mat1.m[1][2]*mat2.m[2][0];
	t.m[1][1] = mat1.m[1][0]*mat2.m[0][1] + mat1.m[1][1]*mat2.m[1][1] + mat1.m[1][2]*mat2.m[2][1];
	t.m[1][2] = mat1.m[1][0]*mat2.m[0][2] + mat1.m[1][1]*mat2.m[1][2] + mat1.m[1][2]*mat2.m[2][2];
	t.m[2][0] = mat1.m[2][0]*mat2.m[0][0] + mat1.m[2][1]*mat2.m[1][0] + mat1.m[2][2]*mat2.m[2][0];
	t.m[2][1] = mat1.m[2][0]*mat2.m[0][1] + mat1.m[2][1]*mat2.m[1][1] + mat1.m[2][2]*mat2.m[2][1];
	t.m[2][2] = mat1.m[2][0]*mat2.m[0][2] + mat1.m[2][1]*mat2.m[1][2] + mat1.m[2][2]*mat2.m[2][2];
	t.m[3][0] = mat1.m[3][0]*mat2.m[0][0] + mat1.m[3][1]*mat2.m[1][0] + mat1.m[3][2]*mat2.m[2][0] + mat2.m[3][0];
	t.m[3][1] = mat1.m[3][0]*mat2.m[0][1] + mat1.m[3][1]*mat2.m[1][1] + mat1.m[3][2]*mat2.m[2][1] + mat2.m[3][1];
	t.m[3][2] = mat1.m[3][0]*mat2.m[0][2] + mat1.m[3][1]*mat2.m[1][2] + mat1.m[3][2]*mat2.m[2][2] + mat2.m[3][2];

	m[0][0] = t.m[0][0]; m[0][1] = t.m[0][1]; m[0][2] = t.m[0][2];
	m[1][0] = t.m[1][0]; m[1][1] = t.m[1][1]; m[1][2] = t.m[1][2];
	m[2][0] = t.m[2][0]; m[2][1] = t.m[2][1]; m[2][2] = t.m[2][2];
	m[3][0] = t.m[3][0]; m[3][1] = t.m[3][1]; m[3][2] = t.m[3][2];

	return *this;
}

Matrix& Matrix::Multiply4x4(const Matrix &mat)
{
	Matrix t;

	t.m[0][0] = m[0][0]*mat.m[0][0] + m[0][1]*mat.m[1][0] + m[0][2]*mat.m[2][0] + m[0][3]*mat.m[3][0];
	t.m[0][1] = m[0][0]*mat.m[0][1] + m[0][1]*mat.m[1][1] + m[0][2]*mat.m[2][1] + m[0][3]*mat.m[3][1];
	t.m[0][2] = m[0][0]*mat.m[0][2] + m[0][1]*mat.m[1][2] + m[0][2]*mat.m[2][2] + m[0][3]*mat.m[3][2];
	t.m[0][3] = m[0][0]*mat.m[0][3] + m[0][1]*mat.m[1][3] + m[0][2]*mat.m[2][3] + m[0][3]*mat.m[3][3];
	t.m[1][0] = m[1][0]*mat.m[0][0] + m[1][1]*mat.m[1][0] + m[1][2]*mat.m[2][0] + m[1][3]*mat.m[3][0];
	t.m[1][1] = m[1][0]*mat.m[0][1] + m[1][1]*mat.m[1][1] + m[1][2]*mat.m[2][1] + m[1][3]*mat.m[3][1];
	t.m[1][2] = m[1][0]*mat.m[0][2] + m[1][1]*mat.m[1][2] + m[1][2]*mat.m[2][2] + m[1][3]*mat.m[3][2];
	t.m[1][3] = m[1][0]*mat.m[0][3] + m[1][1]*mat.m[1][3] + m[1][2]*mat.m[2][3] + m[1][3]*mat.m[3][3];
	t.m[2][0] = m[2][0]*mat.m[0][0] + m[2][1]*mat.m[1][0] + m[2][2]*mat.m[2][0] + m[2][3]*mat.m[3][0];
	t.m[2][1] = m[2][0]*mat.m[0][1] + m[2][1]*mat.m[1][1] + m[2][2]*mat.m[2][1] + m[2][3]*mat.m[3][1];
	t.m[2][2] = m[2][0]*mat.m[0][2] + m[2][1]*mat.m[1][2] + m[2][2]*mat.m[2][2] + m[2][3]*mat.m[3][2];
	t.m[2][3] = m[2][0]*mat.m[0][3] + m[2][1]*mat.m[1][3] + m[2][2]*mat.m[2][3] + m[2][3]*mat.m[3][3];
	t.m[3][0] = m[3][0]*mat.m[0][0] + m[3][1]*mat.m[1][0] + m[3][2]*mat.m[2][0] + m[3][3]*mat.m[3][0];
	t.m[3][1] = m[3][0]*mat.m[0][1] + m[3][1]*mat.m[1][1] + m[3][2]*mat.m[2][1] + m[3][3]*mat.m[3][1];
	t.m[3][2] = m[3][0]*mat.m[0][2] + m[3][1]*mat.m[1][2] + m[3][2]*mat.m[2][2] + m[3][3]*mat.m[3][2];
	t.m[3][3] = m[3][0]*mat.m[0][3] + m[3][1]*mat.m[1][3] + m[3][2]*mat.m[2][3] + m[3][3]*mat.m[3][3];

	m[0][0] = t.m[0][0]; m[0][1] = t.m[0][1]; m[0][2] = t.m[0][2]; m[0][3] = t.m[0][3];
	m[1][0] = t.m[1][0]; m[1][1] = t.m[1][1]; m[1][2] = t.m[1][2]; m[1][3] = t.m[1][3];
	m[2][0] = t.m[2][0]; m[2][1] = t.m[2][1]; m[2][2] = t.m[2][2]; m[2][3] = t.m[2][3];
	m[3][0] = t.m[3][0]; m[3][1] = t.m[3][1]; m[3][2] = t.m[3][2]; m[3][3] = t.m[3][3];

	return *this;
}

Matrix& Matrix::Multiply4x4(const Matrix &mat1, const Matrix &mat2)
{
	Matrix t;

	t.m[0][0] = mat1.m[0][0]*mat2.m[0][0] + mat1.m[0][1]*mat2.m[1][0] + mat1.m[0][2]*mat2.m[2][0] + mat1.m[0][3]*mat2.m[3][0];
	t.m[0][1] = mat1.m[0][0]*mat2.m[0][1] + mat1.m[0][1]*mat2.m[1][1] + mat1.m[0][2]*mat2.m[2][1] + mat1.m[0][3]*mat2.m[3][1];
	t.m[0][2] = mat1.m[0][0]*mat2.m[0][2] + mat1.m[0][1]*mat2.m[1][2] + mat1.m[0][2]*mat2.m[2][2] + mat1.m[0][3]*mat2.m[3][2];
	t.m[0][3] = mat1.m[0][0]*mat2.m[0][3] + mat1.m[0][1]*mat2.m[1][3] + mat1.m[0][2]*mat2.m[2][3] + mat1.m[0][3]*mat2.m[3][3];
	t.m[1][0] = mat1.m[1][0]*mat2.m[0][0] + mat1.m[1][1]*mat2.m[1][0] + mat1.m[1][2]*mat2.m[2][0] + mat1.m[1][3]*mat2.m[3][0];
	t.m[1][1] = mat1.m[1][0]*mat2.m[0][1] + mat1.m[1][1]*mat2.m[1][1] + mat1.m[1][2]*mat2.m[2][1] + mat1.m[1][3]*mat2.m[3][1];
	t.m[1][2] = mat1.m[1][0]*mat2.m[0][2] + mat1.m[1][1]*mat2.m[1][2] + mat1.m[1][2]*mat2.m[2][2] + mat1.m[1][3]*mat2.m[3][2];
	t.m[1][3] = mat1.m[1][0]*mat2.m[0][3] + mat1.m[1][1]*mat2.m[1][3] + mat1.m[1][2]*mat2.m[2][3] + mat1.m[1][3]*mat2.m[3][3];
	t.m[2][0] = mat1.m[2][0]*mat2.m[0][0] + mat1.m[2][1]*mat2.m[1][0] + mat1.m[2][2]*mat2.m[2][0] + mat1.m[2][3]*mat2.m[3][0];
	t.m[2][1] = mat1.m[2][0]*mat2.m[0][1] + mat1.m[2][1]*mat2.m[1][1] + mat1.m[2][2]*mat2.m[2][1] + mat1.m[2][3]*mat2.m[3][1];
	t.m[2][2] = mat1.m[2][0]*mat2.m[0][2] + mat1.m[2][1]*mat2.m[1][2] + mat1.m[2][2]*mat2.m[2][2] + mat1.m[2][3]*mat2.m[3][2];
	t.m[2][3] = mat1.m[2][0]*mat2.m[0][3] + mat1.m[2][1]*mat2.m[1][3] + mat1.m[2][2]*mat2.m[2][3] + mat1.m[2][3]*mat2.m[3][3];
	t.m[3][0] = mat1.m[3][0]*mat2.m[0][0] + mat1.m[3][1]*mat2.m[1][0] + mat1.m[3][2]*mat2.m[2][0] + mat1.m[3][3]*mat2.m[3][0];
	t.m[3][1] = mat1.m[3][0]*mat2.m[0][1] + mat1.m[3][1]*mat2.m[1][1] + mat1.m[3][2]*mat2.m[2][1] + mat1.m[3][3]*mat2.m[3][1];
	t.m[3][2] = mat1.m[3][0]*mat2.m[0][2] + mat1.m[3][1]*mat2.m[1][2] + mat1.m[3][2]*mat2.m[2][2] + mat1.m[3][3]*mat2.m[3][2];
	t.m[3][3] = mat1.m[3][0]*mat2.m[0][3] + mat1.m[3][1]*mat2.m[1][3] + mat1.m[3][2]*mat2.m[2][3] + mat1.m[3][3]*mat2.m[3][3];

	m[0][0] = t.m[0][0]; m[0][1] = t.m[0][1]; m[0][2] = t.m[0][2]; m[0][3] = t.m[0][3];
	m[1][0] = t.m[1][0]; m[1][1] = t.m[1][1]; m[1][2] = t.m[1][2]; m[1][3] = t.m[1][3];
	m[2][0] = t.m[2][0]; m[2][1] = t.m[2][1]; m[2][2] = t.m[2][2]; m[2][3] = t.m[2][3];
	m[3][0] = t.m[3][0]; m[3][1] = t.m[3][1]; m[3][2] = t.m[3][2]; m[3][3] = t.m[3][3];

	return *this;
}

Matrix& Matrix::Multiply3x3(const Matrix &mat)
{
	Matrix t;

	t.m[0][0] = m[0][0]*mat.m[0][0] + m[0][1]*mat.m[1][0] + m[0][2]*mat.m[2][0];
	t.m[0][1] = m[0][0]*mat.m[0][1] + m[0][1]*mat.m[1][1] + m[0][2]*mat.m[2][1];
	t.m[0][2] = m[0][0]*mat.m[0][2] + m[0][1]*mat.m[1][2] + m[0][2]*mat.m[2][2];
	t.m[1][0] = m[1][0]*mat.m[0][0] + m[1][1]*mat.m[1][0] + m[1][2]*mat.m[2][0];
	t.m[1][1] = m[1][0]*mat.m[0][1] + m[1][1]*mat.m[1][1] + m[1][2]*mat.m[2][1];
	t.m[1][2] = m[1][0]*mat.m[0][2] + m[1][1]*mat.m[1][2] + m[1][2]*mat.m[2][2];
	t.m[2][0] = m[2][0]*mat.m[0][0] + m[2][1]*mat.m[1][0] + m[2][2]*mat.m[2][0];
	t.m[2][1] = m[2][0]*mat.m[0][1] + m[2][1]*mat.m[1][1] + m[2][2]*mat.m[2][1];
	t.m[2][2] = m[2][0]*mat.m[0][2] + m[2][1]*mat.m[1][2] + m[2][2]*mat.m[2][2];

	m[0][0] = t.m[0][0]; m[0][1] = t.m[0][1]; m[0][2] = t.m[0][2];
	m[1][0] = t.m[1][0]; m[1][1] = t.m[1][1]; m[1][2] = t.m[1][2];
	m[2][0] = t.m[2][0]; m[2][1] = t.m[2][1]; m[2][2] = t.m[2][2];

	return *this;
}

Matrix& Matrix::Multiply3x3(const Matrix &mat1, const Matrix &mat2)
{
	Matrix t;

	t.m[0][0] = mat1.m[0][0]*mat2.m[0][0] + mat1.m[0][1]*mat2.m[1][0] + mat1.m[0][2]*mat2.m[2][0];
	t.m[0][1] = mat1.m[0][0]*mat2.m[0][1] + mat1.m[0][1]*mat2.m[1][1] + mat1.m[0][2]*mat2.m[2][1];
	t.m[0][2] = mat1.m[0][0]*mat2.m[0][2] + mat1.m[0][1]*mat2.m[1][2] + mat1.m[0][2]*mat2.m[2][2];
	t.m[1][0] = mat1.m[1][0]*mat2.m[0][0] + mat1.m[1][1]*mat2.m[1][0] + mat1.m[1][2]*mat2.m[2][0];
	t.m[1][1] = mat1.m[1][0]*mat2.m[0][1] + mat1.m[1][1]*mat2.m[1][1] + mat1.m[1][2]*mat2.m[2][1];
	t.m[1][2] = mat1.m[1][0]*mat2.m[0][2] + mat1.m[1][1]*mat2.m[1][2] + mat1.m[1][2]*mat2.m[2][2];
	t.m[2][0] = mat1.m[2][0]*mat2.m[0][0] + mat1.m[2][1]*mat2.m[1][0] + mat1.m[2][2]*mat2.m[2][0];
	t.m[2][1] = mat1.m[2][0]*mat2.m[0][1] + mat1.m[2][1]*mat2.m[1][1] + mat1.m[2][2]*mat2.m[2][1];
	t.m[2][2] = mat1.m[2][0]*mat2.m[0][2] + mat1.m[2][1]*mat2.m[1][2] + mat1.m[2][2]*mat2.m[2][2];

	m[0][0] = t.m[0][0]; m[0][1] = t.m[0][1]; m[0][2] = t.m[0][2];
	m[1][0] = t.m[1][0]; m[1][1] = t.m[1][1]; m[1][2] = t.m[1][2];
	m[2][0] = t.m[2][0]; m[2][1] = t.m[2][1]; m[2][2] = t.m[2][2];

	return *this;
}

// matrix inverse shamelessly taken from some math book...
#define ACCUMULATE    \
	if(temp >= 0.0)   \
		pos += temp;  \
	else              \
		neg += temp;

#define PRECISION_LIMIT (1.0e-10)

Matrix& Matrix::Inverse(const Matrix &mat)
{
	Matrix out;
	register float det_1;
	float pos, neg, temp;

	//	* Calculate the determinant of submatrix A and determine if the
	//	* the matrix is singular as limited by the double precision
	//	* floating-point data representation.
	pos = neg = 0.0;
	temp =  mat.m[0][0] * mat.m[1][1] * mat.m[2][2];
	ACCUMULATE
	temp =  mat.m[0][1] * mat.m[1][2] * mat.m[2][0];
	ACCUMULATE
	temp =  mat.m[0][2] * mat.m[1][0] * mat.m[2][1];
	ACCUMULATE
	temp = -mat.m[0][2] * mat.m[1][1] * mat.m[2][0];
	ACCUMULATE
	temp = -mat.m[0][1] * mat.m[1][0] * mat.m[2][2];
	ACCUMULATE
	temp = -mat.m[0][0] * mat.m[1][2] * mat.m[2][1];
	ACCUMULATE
	det_1 = pos + neg;

	// Is the submatrix A singular?
	if ((det_1 == 0.0f) || (fabsf(det_1 / (pos - neg)) < PRECISION_LIMIT))
	{
		// Matrix M has no inverse
		LOGD("Matrix::Inverse: Singular matrix (Matrix has no inverse)...\n");
		return *this;
	}

	// Calculate inverse(A) = adj(A) / det(A)
	det_1 = 1.0f / det_1;
	out.m[0][0] =  (mat.m[1][1]*mat.m[2][2] - mat.m[1][2]*mat.m[2][1]) * det_1;
	out.m[1][0] = -(mat.m[1][0]*mat.m[2][2] - mat.m[1][2]*mat.m[2][0]) * det_1;
	out.m[2][0] =  (mat.m[1][0]*mat.m[2][1] - mat.m[1][1]*mat.m[2][0]) * det_1;
	out.m[0][1] = -(mat.m[0][1]*mat.m[2][2] - mat.m[0][2]*mat.m[2][1]) * det_1;
	out.m[1][1] =  (mat.m[0][0]*mat.m[2][2] - mat.m[0][2]*mat.m[2][0]) * det_1;
	out.m[2][1] = -(mat.m[0][0]*mat.m[2][1] - mat.m[0][1]*mat.m[2][0]) * det_1;
	out.m[0][2] =  (mat.m[0][1]*mat.m[1][2] - mat.m[0][2]*mat.m[1][1]) * det_1;
	out.m[1][2] = -(mat.m[0][0]*mat.m[1][2] - mat.m[0][2]*mat.m[1][0]) * det_1;
	out.m[2][2] =  (mat.m[0][0]*mat.m[1][1] - mat.m[0][1]*mat.m[1][0]) * det_1;

	// Calculate -C * inverse(A)
	out.m[3][0] = -(mat.m[3][0]*out.m[0][0] + mat.m[3][1]*out.m[1][0] + mat.m[3][2]*out.m[2][0]);
	out.m[3][1] = -(mat.m[3][0]*out.m[0][1] + mat.m[3][1]*out.m[1][1] + mat.m[3][2]*out.m[2][1]);
	out.m[3][2] = -(mat.m[3][0]*out.m[0][2] + mat.m[3][1]*out.m[1][2] + mat.m[3][2]*out.m[2][2]);

	// Fill in last column
	out.m[0][3] = out.m[1][3] = out.m[2][3] = 0.0f;
	out.m[3][3] = 1.0f;

	return *this = out;
}

Vector4 Matrix::CalculateQuaternion()
{
	Vector4 t;
	float T = m[0][0] + m[1][1] + m[2][2];
	if(T>0.0f)
	{
//		If the trace of the matrix is greater than zero, then
//		perform an "instant" calculation.
		float S=0.5f/(float)sqrt(T+1);
		t.w = 0.25f/S;
		t.x = (m[2][1] - m[1][2])*S;
		t.y = (m[0][2] - m[2][0])*S;
		t.z = (m[1][0] - m[0][1])*S;
	}
	else
	{
//		If the trace of the matrix is less than or equal to zero
//		then identify which major diagonal element has the greatest
//		value.
//		Depending on this value, calculate the following:
		if ((m[0][0] > m[1][1]) && (m[0][0] > m[2][2]))
		{
			float S = (float)sqrt(1.0f + m[0][0] - m[1][1] - m[2][2]) * 2.0f;
			t.x = 0.25f * S;
			t.y = (m[0][1] + m[1][0] ) / S;
			t.z = (m[0][2] + m[2][0] ) / S;
			t.w = (m[1][2] - m[2][1] ) / S;
		}
		else if (m[1][1] > m[2][2])
		{
			float S = (float)sqrt(1.0f + m[1][1] - m[0][0] - m[2][2]) * 2.0f;
			t.x = (m[0][1] + m[1][0]) / S;
			t.y = 0.25f * S;
			t.z = (m[1][2] + m[2][1]) / S;
			t.w = (m[0][2] - m[2][0]) / S;
		}
		else
		{ 
			float S = (float)sqrt(1.0f + m[2][2] - m[0][0] - m[1][1] ) * 2.0f;
			t.x = (m[0][2] + m[2][0]) / S;
			t.y = (m[1][2] + m[2][1]) / S;
			t.z = 0.25f * S;
			t.w = (m[0][1] - m[1][0]) / S;
		}
	}

	return t;
}

