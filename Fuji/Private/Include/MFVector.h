#if !defined(_MFVECTOR_H)
#define _MFVECTOR_H

#if defined(_MSC_VER)
#pragma warning(disable:4201)
#endif

class MFMatrix;

MFALIGN_BEGIN(16)
class MFVector
{
public:
	struct
	{
		float x,y,z,w;
	};

	static const MFVector zero;
	static const MFVector one;
	static const MFVector identity;
	static const MFVector up;

	// general purpose vector operators
	MFVector operator-();

	bool operator==(const MFVector &v) const;
	bool operator!=(const MFVector &v) const;

	MFVector& operator=(const MFVector &v);
	MFVector& operator+=(const MFVector &v);
	MFVector& operator-=(const MFVector &v);
	MFVector& operator*=(float f);
	MFVector& operator*=(const MFVector &v);
//	MFVector& operator/=(float f);			// Since division operators shoudl NEVER really be used anyway, i wont expose them..
//	MFVector& operator/=(const MFVector &v);	// teach people to write code properly ;)

	MFVector operator+(const MFVector &v) const;
	MFVector operator-(const MFVector &v) const;
	MFVector operator*(float f) const;
	MFVector operator*(const MFVector &v) const;
//	MFVector operator/(float f) const;
//	MFVector operator/(const MFVector &v) const;

	// these are some functions for the fastest possible implementations on all platforms
	// NOTE: These functions do NOT preserve the 'w' component!
	MFVector& Add3(const MFVector &v1, const MFVector &v2);
	MFVector& Sub3(const MFVector &v1, const MFVector &v2);
	MFVector& Mul3(const MFVector &v1, float f);
	MFVector& Mul3(const MFVector &v1, const MFVector &v2);
	MFVector& Mad3(const MFVector &v1, float f, const MFVector &v3);
	MFVector& Mad3(const MFVector &v1, const MFVector &v2, const MFVector &v3);
//	MFVector& Div3(const MFVector &v1, float f);
//	MFVector& Div3(const MFVector &v1, const MFVector &v2);

	// NOTE: These functions do NOT preserve the 'z' or 'w' components!
	MFVector& Add2(const MFVector &v1, const MFVector &v2);
	MFVector& Sub2(const MFVector &v1, const MFVector &v2);
	MFVector& Mul2(const MFVector &v1, float f);
	MFVector& Mul2(const MFVector &v1, const MFVector &v2);
	MFVector& Mad2(const MFVector &v1, float f, const MFVector &v3);
	MFVector& Mad2(const MFVector &v1, const MFVector &v2, const MFVector &v3);
//	MFVector& Div2(const MFVector &v1, float f);
//	MFVector& Div2(const MFVector &v1, const MFVector &v2);

	operator float*();
	operator float*() const;

	uint32 ToPackedColour() const;			// WARNING: this function produces different results on all hardware
	MFVector& FromPackedColour(uint32 col);

	float Dot4(const MFVector &vec) const;
	float DotH(const MFVector &vec4) const;
	float Dot3(const MFVector &vec) const;
	float Dot2(const MFVector &vec) const;

//	MFVector Cross4(const MFVector &vec) const;
	MFVector Cross3(const MFVector &vec) const;
	float Cross2(const MFVector &vec) const;

//	MFVector& Cross4(const MFVector &vec, const MFVector &vec2);
	MFVector& Cross3(const MFVector &vec, const MFVector &vec2);

/*
	MFVector& ApplyMatrix(const Matrix &matrix);
	MFVector& ApplyMatrixH(const Matrix &matrix);
	MFVector& ApplyMatrix3x3(const Matrix &matrix);
*/

	float MagSquared4() const;
	float MagSquared3() const;
	float MagSquared2() const;
	float Magnitude4() const;
	float Magnitude3() const;
	float Magnitude2() const;
	MFVector& Normalise4();
	MFVector& Normalise3();
	MFVector& Normalise2();

	float Distance(const MFVector &v) const;
	float GetAngle(const MFVector &ref = up);

	MFVector& Lerp(const MFVector &v, float t);

	char* ToString4() const;
	char* ToString3() const;
	char* ToString2() const;
}
MFALIGN_END(16);

MFVector operator*(float f, const MFVector &v);

MFVector MakeVector(float f);
MFVector MakeVector(float x, float y, float z = 0.0f, float w = 1.0f);
MFVector MakeVector(const MFVector &v, float w);

MFVector MFMin(const MFVector &a, const MFVector &b);
MFVector MFMax(const MFVector &a, const MFVector &b);

#include "MFVector.inl"

#endif
