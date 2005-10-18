#if !defined(_MFMATRIX_H)
#define _MFMATRIX_H

#include "MFVector.h"

MFALIGN_BEGIN(16)
class MFMatrix
{
public:
	float m[4][4];

	static const MFMatrix identity;

	MFMatrix& operator=(const MFMatrix& mat);
	bool operator==(const MFMatrix& mat) const;
	bool operator!=(const MFMatrix& mat) const;

	MFMatrix& SetIdentity();

	MFMatrix& Translate(const MFVector &trans);
	MFMatrix& Rotate(const MFVector &axis, float angle);
	MFMatrix& RotateQ(const MFVector &q);
	MFMatrix& RotateYPR(float yaw, float pitch, float roll);
	MFMatrix& RotateX(float angle);
	MFMatrix& RotateY(float angle);
	MFMatrix& RotateZ(float angle);
	MFMatrix& Scale(const MFVector& scale);

	MFMatrix& SetTranslation(const MFVector &trans);
	MFMatrix& SetRotation(const MFVector &axis, float angle);
	MFMatrix& SetRotationQ(const MFVector &q);
	MFMatrix& SetRotationYPR(float yaw, float pitch, float roll);
	MFMatrix& SetRotationX(float angle);
	MFMatrix& SetRotationY(float angle);
	MFMatrix& SetRotationZ(float angle);
	MFMatrix& SetScale(const MFVector& scale);

	MFMatrix& LookAt(const MFVector& pos, const MFVector& at, const MFVector& up = MFVector::up);

	MFMatrix& Transpose();
	MFMatrix& Transpose(const MFMatrix &matrix);
	MFMatrix& Transpose3x3();
	MFMatrix& Transpose3x3(const MFMatrix &matrix);

	MFMatrix& Multiply(const MFMatrix& mat);
	MFMatrix& Multiply(const MFMatrix& mat1, const MFMatrix& mat2);
	MFMatrix& Multiply4x4(const MFMatrix& mat);
	MFMatrix& Multiply4x4(const MFMatrix& mat1, const MFMatrix& mat2);
	MFMatrix& Multiply3x3(const MFMatrix& mat);
	MFMatrix& Multiply3x3(const MFMatrix& mat1, const MFMatrix& mat2);

	MFVector TransformVector(const MFVector& vec) const;
	MFVector TransformVectorH(const MFVector& vec) const;
	MFVector TransformVector3(const MFVector& vec) const;

	MFMatrix& Inverse() { return Inverse(*this); }
	MFMatrix& Inverse(const MFMatrix &matrix);

	MFVector CalculateQuaternion();

	MFMatrix& ClearW();

	const MFVector& GetXAxis() const;
	const MFVector& GetYAxis() const;
	const MFVector& GetZAxis() const;
	const MFVector& GetTrans() const;

	void SetXAxis3(const MFVector &v);
	void SetYAxis3(const MFVector &v);
	void SetZAxis3(const MFVector &v);
	void SetTrans3(const MFVector &v);
	void SetXAxis4(const MFVector &v);
	void SetYAxis4(const MFVector &v);
	void SetZAxis4(const MFVector &v);
	void SetTrans4(const MFVector &v);

	char* ToString() const;
}
MFALIGN_END(16);

MFVector ApplyMatrix(const MFVector& vector, const MFMatrix &matrix);
MFVector ApplyMatrixH(const MFVector& vector, const MFMatrix &matrix);
MFVector ApplyMatrix3(const MFVector& vector, const MFMatrix &matrix);

#include "MFMatrix.inl"

#endif
