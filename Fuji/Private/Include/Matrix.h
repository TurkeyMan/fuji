#if !defined(_MATRIX_H)
#define _MATRIX_H

#include "Vector3.h"
#include "Vector4.h"

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
	Matrix& Scale(const Vector3& scale);

	Matrix& SetTranslation(const Vector3 &trans);
	Matrix& SetRotation(const Vector3 &axis, float angle);
	Matrix& SetRotationQ(const Vector4 &q);
	Matrix& SetRotationYPR(float yaw, float pitch, float roll);
	Matrix& SetRotationX(float angle);
	Matrix& SetRotationY(float angle);
	Matrix& SetRotationZ(float angle);
	Matrix& SetScale(const Vector3& scale);

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

	Vector4 CalculateQuaternion();

	inline Matrix& ClearW() { m[0][3]=0.0f; m[1][3]=0.0f; m[2][3]=0.0f; m[3][3]=1.0f; return *this; }

	inline Vector3 GetXAxis3() const { Vector3 t; t.x = m[0][0]; t.y = m[0][1]; t.z = m[0][2]; return t; }
	inline Vector3 GetYAxis3() const { Vector3 t; t.x = m[1][0]; t.y = m[1][1]; t.z = m[1][2]; return t; }
	inline Vector3 GetZAxis3() const { Vector3 t; t.x = m[2][0]; t.y = m[2][1]; t.z = m[2][2]; return t; }
	inline Vector3 GetTrans3() const { Vector3 t; t.x = m[3][0]; t.y = m[3][1]; t.z = m[3][2]; return t; }
	inline Vector4 GetXAxis4() const { Vector4 t; t.x = m[0][0]; t.y = m[0][1]; t.z = m[0][2]; t.w = m[0][3]; return t; }
	inline Vector4 GetYAxis4() const { Vector4 t; t.x = m[1][0]; t.y = m[1][1]; t.z = m[1][2]; t.w = m[1][3]; return t; }
	inline Vector4 GetZAxis4() const { Vector4 t; t.x = m[2][0]; t.y = m[2][1]; t.z = m[2][2]; t.w = m[2][3]; return t; }
	inline Vector4 GetTrans4() const { Vector4 t; t.x = m[3][0]; t.y = m[3][1]; t.z = m[3][2]; t.w = m[3][3]; return t; }

	inline void SetXAxis(const Vector3 &v) { m[0][0] = v.x; m[0][1] = v.y; m[0][2] = v.z; }
	inline void SetYAxis(const Vector3 &v) { m[1][0] = v.x; m[1][1] = v.y; m[1][2] = v.z; }
	inline void SetZAxis(const Vector3 &v) { m[2][0] = v.x; m[2][1] = v.y; m[2][2] = v.z; }
	inline void SetTrans(const Vector3 &v) { m[3][0] = v.x; m[3][1] = v.y; m[3][2] = v.z; }
	inline void SetXAxis(const Vector4 &v) { m[0][0] = v.x; m[0][1] = v.y; m[0][2] = v.z; m[0][3] = v.w; }
	inline void SetYAxis(const Vector4 &v) { m[1][0] = v.x; m[1][1] = v.y; m[1][2] = v.z; m[1][3] = v.w; }
	inline void SetZAxis(const Vector4 &v) { m[2][0] = v.x; m[2][1] = v.y; m[2][2] = v.z; m[2][3] = v.w; }
	inline void SetTrans(const Vector4 &v) { m[3][0] = v.x; m[3][1] = v.y; m[3][2] = v.z; m[3][3] = v.w; }

	inline char* ToString() const { return STR("| %.2f, %.2f, %.2f, %.2f |\n| %.2f, %.2f, %.2f, %.2f |\n| %.2f, %.2f, %.2f, %.2f |\n| %.2f, %.2f, %.2f, %.2f |", m[0][0], m[0][1], m[0][2], m[0][3], m[1][0], m[1][1], m[1][2], m[1][3], m[2][0], m[2][1], m[2][2], m[2][3], m[3][0], m[3][1], m[3][2], m[3][3]); }
};

#endif
