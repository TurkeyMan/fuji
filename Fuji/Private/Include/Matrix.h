#if !defined(_MATRIX_H)
#define _MATRIX_H

class Vector3;
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

	Matrix& Transpose();
	Matrix& Transpose3x3();

	Matrix& Multiply(const Matrix &mat);
	Matrix& Multiply(const Matrix &mat1, const Matrix &mat2);

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
};

#endif