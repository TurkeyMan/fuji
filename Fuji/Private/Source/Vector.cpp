#include "Vector3.h"
#include "Vector4.h"
#include "Matrix.h"

const Vector3 Vector3::zero = {0,0,0,0};
const Vector3 Vector3::one = {1,1,1,0};
const Vector3 Vector3::up = {0,1,0,0};

const Vector4 Vector4::zero = {0,0,0,0};
const Vector4 Vector4::one = {1,1,1,1};


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

