#if !defined(_MATRIX_H)
#define _MATRIX_H

#include "Vector3.h"
class Vector4;

class Matrix
{
public:
	float m[4][4];

	static const Matrix identity;

	Matrix& operator=(const Matrix& mat);
	bool operator==(const Matrix& mat) const;
	bool operator!=(const Matrix& mat) const;

	Matrix& SetIdentity();

	Matrix& Translate(const Vector3 &trans);
	Matrix& Rotate(const Vector3 &axis, float angle);
	Matrix& RotateQ(const Vector3 &axis, float angle);
	Matrix& RotateYPR(float yaw, float pitch, float roll);
	Matrix& RotateX(float angle);
	Matrix& RotateY(float angle);
	Matrix& RotateZ(float angle);

	Matrix& SetTranslation(const Vector3 &trans);
	Matrix& SetRotation(const Vector3 &axis, float angle);
	Matrix& SetRotationQ(const Vector4 &q);
	Matrix& SetRotationYPR(float yaw, float pitch, float roll);
	Matrix& SetRotationX(float angle);
	Matrix& SetRotationY(float angle);
	Matrix& SetRotationZ(float angle);

	Matrix& LookAt(const Vector3& pos, const Vector3& at, const Vector3& up = Vector3::up);

	Matrix& Transpose();
	Matrix& Transpose3x3();

	Matrix& Multiply(const Matrix& mat);
	Matrix& Multiply(const Matrix& mat1, const Matrix& mat2);
	Matrix& Multiply4x4(const Matrix& mat);
	Matrix& Multiply4x4(const Matrix& mat1, const Matrix& mat2);
	Matrix& Multiply3x3(const Matrix& mat);
	Matrix& Multiply3x3(const Matrix& mat1, const Matrix& mat2);

	Vector4 TransformVector(const Vector4& vec);
	Vector3 TransformVector3(const Vector3& vec);
	Vector3 TransformVector3x3(const Vector3& vec);

	Matrix& Inverse();

	Matrix& ClearW();

	Vector4 CalculateQuaternion();

	Vector3 GetXAxis3() const;
	Vector3 GetYAxis3() const;
	Vector3 GetZAxis3() const;
	Vector3 GetTrans3() const;
	Vector4 GetXAxis4() const;
	Vector4 GetYAxis4() const;
	Vector4 GetZAxis4() const;
	Vector4 GetTrans4() const;

	void SetXAxis(const Vector3 &v);
	void SetYAxis(const Vector3 &v);
	void SetZAxis(const Vector3 &v);
	void SetTrans(const Vector3 &v);
	void SetXAxis(const Vector4 &v);
	void SetYAxis(const Vector4 &v);
	void SetZAxis(const Vector4 &v);
	void SetTrans(const Vector4 &v);

	inline char* ToString() const { return STR("| %.2f, %.2f, %.2f, %.2f |\n| %.2f, %.2f, %.2f, %.2f |\n| %.2f, %.2f, %.2f, %.2f |\n| %.2f, %.2f, %.2f, %.2f |", m[0][0], m[0][1], m[0][2], m[0][3], m[1][0], m[1][1], m[1][2], m[1][3], m[2][0], m[2][1], m[2][2], m[2][3], m[3][0], m[3][1], m[3][2], m[3][3]); }
};

#endif