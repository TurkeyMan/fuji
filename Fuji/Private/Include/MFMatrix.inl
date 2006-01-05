// matrix inline functions

inline MFMatrix& MFMatrix::SetIdentity()
{
	*this = MFMatrix::identity;
	return *this;
}

inline MFMatrix& MFMatrix::Translate(const MFVector &trans)
{
	m[3][0] += trans.x;
	m[3][1] += trans.y;
	m[3][2] += trans.z;
	return *this;
}

inline MFVector MFMatrix::TransformVector(const MFVector& vec) const
{
	MFVector t;
	t.x = vec.x*m[0][0] + vec.y*m[1][0] + vec.z*m[2][0] + vec.w*m[3][0];
	t.y = vec.x*m[0][1] + vec.y*m[1][1] + vec.z*m[2][1] + vec.w*m[3][1];
	t.z = vec.x*m[0][2] + vec.y*m[1][2] + vec.z*m[2][2] + vec.w*m[3][2];
	t.w = vec.x*m[0][3] + vec.y*m[1][3] + vec.z*m[2][3] + vec.w*m[3][3];
	return t;
}

inline MFVector MFMatrix::TransformVectorH(const MFVector& vec) const
{
	MFVector t;
/*
	t.Mad3(GetXAxis(), vec.x, GetTrans());
	t.Mad3(GetYAxis(), vec.y, t);
	t.Mad3(GetZAxis(), vec.z, t);
*/
	t.x = vec.x*m[0][0] + vec.y*m[1][0] + vec.z*m[2][0] + m[3][0];
	t.y = vec.x*m[0][1] + vec.y*m[1][1] + vec.z*m[2][1] + m[3][1];
	t.z = vec.x*m[0][2] + vec.y*m[1][2] + vec.z*m[2][2] + m[3][2];

	return t;
}

inline MFVector MFMatrix::TransformVector3(const MFVector& vec) const
{
	MFVector t;
	t.x = vec.x*m[0][0] + vec.y*m[1][0] + vec.z*m[2][0];
	t.y = vec.x*m[0][1] + vec.y*m[1][1] + vec.z*m[2][1];
	t.z = vec.x*m[0][2] + vec.y*m[1][2] + vec.z*m[2][2];
	return t;
}

inline MFMatrix& MFMatrix::ClearW()
{
	m[0][3]=0.0f;
	m[1][3]=0.0f;
	m[2][3]=0.0f;
	m[3][3]=1.0f;
	return *this;
}

inline const MFVector& MFMatrix::GetXAxis() const
{
	return *(MFVector*)&m[0][0];
}

inline const MFVector& MFMatrix::GetYAxis() const
{
	return *(MFVector*)&m[1][0];
}

inline const MFVector& MFMatrix::GetZAxis() const
{
	return *(MFVector*)&m[2][0];
}

inline const MFVector& MFMatrix::GetTrans() const
{
	return *(MFVector*)&m[3][0];
}

inline void MFMatrix::SetXAxis3(const MFVector &v)
{
	m[0][0] = v.x;
	m[0][1] = v.y;
	m[0][2] = v.z;
	m[0][3] = 0.0f;
}

inline void MFMatrix::SetYAxis3(const MFVector &v)
{
	m[1][0] = v.x;
	m[1][1] = v.y;
	m[1][2] = v.z;
	m[1][3] = 0.0f;
}

inline void MFMatrix::SetZAxis3(const MFVector &v)
{
	m[2][0] = v.x;
	m[2][1] = v.y;
	m[2][2] = v.z;
	m[2][3] = 0.0f;
}

inline void MFMatrix::SetTrans3(const MFVector &v)
{
	m[3][0] = v.x;
	m[3][1] = v.y;
	m[3][2] = v.z;
	m[3][3] = 1.0f;
}

inline void MFMatrix::SetXAxis4(const MFVector &v)
{
	*(MFVector*)&m[0][0] = v;
}

inline void MFMatrix::SetYAxis4(const MFVector &v)
{
	*(MFVector*)&m[1][0] = v;
}

inline void MFMatrix::SetZAxis4(const MFVector &v)
{
	*(MFVector*)&m[2][0] = v;
}

inline void MFMatrix::SetTrans4(const MFVector &v)
{
	*(MFVector*)&m[3][0] = v;
}

inline const char * MFMatrix::ToString() const
{
	return MFStr("| %.2f, %.2f, %.2f, %.2f |\n| %.2f, %.2f, %.2f, %.2f |\n| %.2f, %.2f, %.2f, %.2f |\n| %.2f, %.2f, %.2f, %.2f |", m[0][0], m[0][1], m[0][2], m[0][3], m[1][0], m[1][1], m[1][2], m[1][3], m[2][0], m[2][1], m[2][2], m[2][3], m[3][0], m[3][1], m[3][2], m[3][3]);
}

inline MFVector ApplyMatrix(const MFVector &vector, const MFMatrix &mat)
{
	MFVector t;
	t.x = vector.x*mat.m[0][0] + vector.y*mat.m[1][0] + vector.z*mat.m[2][0] + vector.w*mat.m[3][0];
	t.y = vector.x*mat.m[0][1] + vector.y*mat.m[1][1] + vector.z*mat.m[2][1] + vector.w*mat.m[3][1];
	t.z = vector.x*mat.m[0][2] + vector.y*mat.m[1][2] + vector.z*mat.m[2][2] + vector.w*mat.m[3][2];
	t.w = vector.x*mat.m[0][3] + vector.y*mat.m[1][3] + vector.z*mat.m[2][3] + vector.w*mat.m[3][3];
	return t;
}

inline MFVector ApplyMatrixH(const MFVector &vector, const MFMatrix &mat)
{
	MFVector t;
	t.x = vector.x*mat.m[0][0] + vector.y*mat.m[1][0] + vector.z*mat.m[2][0] + mat.m[3][0];
	t.y = vector.x*mat.m[0][1] + vector.y*mat.m[1][1] + vector.z*mat.m[2][1] + mat.m[3][1];
	t.z = vector.x*mat.m[0][2] + vector.y*mat.m[1][2] + vector.z*mat.m[2][2] + mat.m[3][2];
	return t;
}

inline MFVector ApplyMatrix3(const MFVector &vector, const MFMatrix &mat)
{
	MFVector t;
	t.x = vector.x*mat.m[0][0] + vector.y*mat.m[1][0] + vector.z*mat.m[2][0];
	t.y = vector.x*mat.m[0][1] + vector.y*mat.m[1][1] + vector.z*mat.m[2][1];
	t.z = vector.x*mat.m[0][2] + vector.y*mat.m[1][2] + vector.z*mat.m[2][2];
	return t;
}
