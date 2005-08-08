#include "Common.h"
#include "Vector3.h"
#include "Vector4.h"
#include "MFVector.h"
#include "Matrix.h"

const Vector3 Vector3::zero = Vector(0.0f, 0.0f, 0.0f);
const Vector3 Vector3::one = Vector(1.0f, 1.0f, 1.0f);
const Vector3 Vector3::up = Vector(0.0f, 1.0f, 0.0f);

const Vector4 Vector4::zero = Vector(0.0f, 0.0f, 0.0f, 0.0f);
const Vector4 Vector4::one = Vector(1.0f, 1.0f, 1.0f, 1.0f);
const Vector4 Vector4::identity = Vector(0.0f, 0.0f, 0.0f, 1.0f);

const MFVector MFVector::zero = MakeVector(0.0f, 0.0f, 0.0f, 0.0f);
const MFVector MFVector::one = MakeVector(1.0f, 1.0f, 1.0f, 1.0f);
const MFVector MFVector::identity = MakeVector(0.0f, 0.0f, 0.0f, 1.0f);
const MFVector MFVector::up = MakeVector(0.0f, 1.0f, 0.0f, 1.0f);

Vector3& Vector3::ApplyMatrix(const Matrix &mat)
{
	float _x=x, _y=y, _z=z;

	x = _x*mat.m[0][0] + _y*mat.m[1][0] + _z*mat.m[2][0] + mat.m[3][0];
	y = _x*mat.m[0][1] + _y*mat.m[1][1] + _z*mat.m[2][1] + mat.m[3][1];
	z = _x*mat.m[0][2] + _y*mat.m[1][2] + _z*mat.m[2][2] + mat.m[3][2];

	return *this;
}

Vector3& Vector3::ApplyMatrix3x3(const Matrix &mat)
{
	float _x=x, _y=y, _z=z;

	x = _x*mat.m[0][0] + _y*mat.m[1][0] + _z*mat.m[2][0];
	y = _x*mat.m[0][1] + _y*mat.m[1][1] + _z*mat.m[2][1];
	z = _x*mat.m[0][2] + _y*mat.m[1][2] + _z*mat.m[2][2];

	return *this;
}

Vector4& Vector4::ApplyMatrix(const Matrix &mat)
{
	float _x=x, _y=y, _z=z, _w=w;

	x = _x*mat.m[0][0] + _y*mat.m[1][0] + _z*mat.m[2][0] + _w*mat.m[3][0];
	y = _x*mat.m[0][1] + _y*mat.m[1][1] + _z*mat.m[2][1] + _w*mat.m[3][1];
	z = _x*mat.m[0][2] + _y*mat.m[1][2] + _z*mat.m[2][2] + _w*mat.m[3][2];
	w = _x*mat.m[0][3] + _y*mat.m[1][3] + _z*mat.m[2][3] + _w*mat.m[3][3];

	return *this;
}

Vector4& Vector4::ApplyMatrix3x3(const Matrix &mat)
{
	float _x=x, _y=y, _z=z;

	x = _x*mat.m[0][0] + _y*mat.m[1][0] + _z*mat.m[2][0];
	y = _x*mat.m[0][1] + _y*mat.m[1][1] + _z*mat.m[2][1];
	z = _x*mat.m[0][2] + _y*mat.m[1][2] + _z*mat.m[2][2];

	return *this;
}

