#include "Common.h"
#include "Matrix.h"
#include "Vector3.h"
#include "Vector4.h"

const Matrix Matrix::identity = 
{
	{
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
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
	DBGASSERT(false, "Not Written...");
	return *this;
}

Matrix& Matrix::RotateX(float angle)
{
	Matrix rot;
	rot.SetRotationX(angle);
	Multiply3x3(rot);
	return *this;
}

Matrix& Matrix::RotateY(float angle)
{
	Matrix rot;
	rot.SetRotationY(angle);
	Multiply3x3(rot);
	return *this;
}

Matrix& Matrix::RotateZ(float angle)
{
	Matrix rot;
	rot.SetRotationZ(angle);
	Multiply3x3(rot);
	return *this;
}

Matrix& Matrix::SetTranslation(const Vector3 &trans)
{
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

  return *this;
}

Matrix& Matrix::SetRotationQ(const Vector4 &q)
{
	DBGASSERT(false, "Not Written...");
	return *this;
}

Matrix& Matrix::SetRotationYPR(float yaw, float pitch, float roll)
{
#if defined(_LINUX) || defined(_DC) /* Not sure if this works yet */
	float sin_yaw, cos_yaw, sin_pitch, cos_pitch, sin_roll, cos_roll;

	sin_yaw = MFSin(yaw);
	cos_yaw = MFCos(yaw);
	sin_pitch = MFSin(pitch);
	cos_pitch = MFCos(pitch);
	sin_roll = MFSin(roll);
	cos_roll = MFCos(roll);

	m[0][0] = cos_roll * cos_yaw - sin_pitch * sin_roll * sin_yaw;
	m[0][1] = -cos_pitch * sin_roll;
	m[0][2] = -cos_yaw * sin_pitch * sin_roll + cos_roll * sin_yaw;
	m[0][3] = 0;

	m[1][0] = cos_yaw * sin_roll + cos_roll * sin_pitch * sin_yaw;
	m[1][1] = cos_pitch * cos_roll;
	m[1][2] = -cos_roll * cos_yaw * sin_pitch + sin_roll + sin_yaw;
	m[1][3] = 0;

	m[2][0] = -cos_pitch * sin_yaw;
	m[2][1] = sin_pitch;
	m[2][2] = cos_pitch * cos_yaw;
	m[2][3] = 0;

	m[3][0] = m[3][1] = m[3][2] = 0;
	m[3][3] = 1;
#else

	D3DXMatrixRotationYawPitchRoll((D3DXMATRIX*)this, yaw, pitch, roll);
//	DBGASSERT(false, "Not Written...");
#endif	
	
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

Matrix& Matrix::Transpose3x3()
{
	register float t;
	t=m[1][0]; m[1][0]=m[0][1]; m[0][1]=t;
	t=m[2][0]; m[2][0]=m[0][2]; m[0][2]=t;
	t=m[2][1]; m[2][1]=m[1][2]; m[1][2]=t;
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

Matrix& Matrix::Inverse()
{
	Matrix tempm = *this;

	tempm.Transpose3x3();
	tempm.m[3][0] = -m[3][0]*m[0][0] + m[3][1]*m[0][1] + m[3][2]*m[0][2] + m[3][3]*m[0][3]; //pMatrix->Translation4().Dot(pMatrix->XAxis4());
	tempm.m[3][1] = -m[3][0]*m[1][0] + m[3][1]*m[1][1] + m[3][2]*m[1][2] + m[3][3]*m[1][3]; //pMatrix->Translation4().Dot(pMatrix->YAxis4());
	tempm.m[3][2] = -m[3][0]*m[2][0] + m[3][1]*m[2][1] + m[3][2]*m[2][2] + m[3][3]*m[2][3]; //pMatrix->Translation4().Dot(pMatrix->ZAxis4());
	tempm.ClearW();
	*this = tempm;

	return *this;
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

