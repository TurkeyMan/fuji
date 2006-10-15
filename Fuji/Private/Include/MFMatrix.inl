// matrix inline functions

inline MFMatrix& MFMatrix::SetIdentity()
{
	*this = MFMatrix::identity;
	return *this;
}

inline MFMatrix& MFMatrix::Translate(const MFVector &trans)
{
	m[12] += trans.x;
	m[13] += trans.y;
	m[14] += trans.z;
	return *this;
}

inline MFVector MFMatrix::TransformVector(const MFVector& vec) const
{
	MFVector t;
	t.x = vec.x*m[0] + vec.y*m[4] + vec.z*m[8] + vec.w*m[12];
	t.y = vec.x*m[1] + vec.y*m[5] + vec.z*m[9] + vec.w*m[13];
	t.z = vec.x*m[2] + vec.y*m[6] + vec.z*m[10] + vec.w*m[14];
	t.w = vec.x*m[3] + vec.y*m[7] + vec.z*m[11] + vec.w*m[15];
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
	t.x = vec.x*m[0] + vec.y*m[4] + vec.z*m[8] + m[12];
	t.y = vec.x*m[1] + vec.y*m[5] + vec.z*m[9] + m[13];
	t.z = vec.x*m[2] + vec.y*m[6] + vec.z*m[10] + m[14];

	return t;
}

inline MFVector MFMatrix::TransformVector3(const MFVector& vec) const
{
	MFVector t;
	t.x = vec.x*m[0] + vec.y*m[4] + vec.z*m[8];
	t.y = vec.x*m[1] + vec.y*m[5] + vec.z*m[9];
	t.z = vec.x*m[2] + vec.y*m[6] + vec.z*m[10];
	return t;
}

inline MFMatrix& MFMatrix::ClearW()
{
	m[3]=0.0f;
	m[7]=0.0f;
	m[11]=0.0f;
	m[15]=1.0f;
	return *this;
}

inline const MFVector& MFMatrix::GetXAxis() const
{
	return *(MFVector*)&m[1];
}

inline const MFVector& MFMatrix::GetYAxis() const
{
	return *(MFVector*)&m[4];
}

inline const MFVector& MFMatrix::GetZAxis() const
{
	return *(MFVector*)&m[8];
}

inline const MFVector& MFMatrix::GetTrans() const
{
	return *(MFVector*)&m[12];
}

inline void MFMatrix::SetXAxis3(const MFVector &v)
{
	m[0] = v.x;
	m[1] = v.y;
	m[2] = v.z;
	m[3] = 0.0f;
}

inline void MFMatrix::SetYAxis3(const MFVector &v)
{
	m[4] = v.x;
	m[5] = v.y;
	m[6] = v.z;
	m[7] = 0.0f;
}

inline void MFMatrix::SetZAxis3(const MFVector &v)
{
	m[8] = v.x;
	m[9] = v.y;
	m[10] = v.z;
	m[11] = 0.0f;
}

inline void MFMatrix::SetTrans3(const MFVector &v)
{
	m[12] = v.x;
	m[13] = v.y;
	m[14] = v.z;
	m[15] = 1.0f;
}

inline void MFMatrix::SetXAxis4(const MFVector &v)
{
	*(MFVector*)&m[0] = v;
}

inline void MFMatrix::SetYAxis4(const MFVector &v)
{
	*(MFVector*)&m[4] = v;
}

inline void MFMatrix::SetZAxis4(const MFVector &v)
{
	*(MFVector*)&m[8] = v;
}

inline void MFMatrix::SetTrans4(const MFVector &v)
{
	*(MFVector*)&m[12] = v;
}

inline const char * MFMatrix::ToString() const
{
	return MFStr("| %.2f, %.2f, %.2f, %.2f |\n| %.2f, %.2f, %.2f, %.2f |\n| %.2f, %.2f, %.2f, %.2f |\n| %.2f, %.2f, %.2f, %.2f |", m[0], m[1], m[2], m[3], m[4], m[5], m[6], m[7], m[8], m[9], m[10], m[11], m[12], m[13], m[14], m[15]);
}

inline MFVector ApplyMatrix(const MFVector &vector, const MFMatrix &mat)
{
	MFVector t;
	t.x = vector.x*mat.m[0] + vector.y*mat.m[4] + vector.z*mat.m[8] + vector.w*mat.m[12];
	t.y = vector.x*mat.m[1] + vector.y*mat.m[5] + vector.z*mat.m[9] + vector.w*mat.m[13];
	t.z = vector.x*mat.m[2] + vector.y*mat.m[6] + vector.z*mat.m[10] + vector.w*mat.m[14];
	t.w = vector.x*mat.m[3] + vector.y*mat.m[7] + vector.z*mat.m[11] + vector.w*mat.m[15];
	return t;
}

inline MFVector ApplyMatrixH(const MFVector &vector, const MFMatrix &mat)
{
	MFVector t;
	t.x = vector.x*mat.m[0] + vector.y*mat.m[4] + vector.z*mat.m[8] + mat.m[12];
	t.y = vector.x*mat.m[1] + vector.y*mat.m[5] + vector.z*mat.m[9] + mat.m[13];
	t.z = vector.x*mat.m[2] + vector.y*mat.m[6] + vector.z*mat.m[10] + mat.m[14];
	return t;
}

inline MFVector ApplyMatrix3(const MFVector &vector, const MFMatrix &mat)
{
	MFVector t;
	t.x = vector.x*mat.m[0] + vector.y*mat.m[4] + vector.z*mat.m[8];
	t.y = vector.x*mat.m[1] + vector.y*mat.m[5] + vector.z*mat.m[9];
	t.z = vector.x*mat.m[2] + vector.y*mat.m[6] + vector.z*mat.m[10];
	return t;
}
