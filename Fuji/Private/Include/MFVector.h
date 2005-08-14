#if !defined(_MFVECTOR_H)
#define _MFVECTOR_H

#if defined(_MSC_VER)
#pragma warning(disable:4201)
#endif

class MFMatrix;

class _ALIGN16 MFVector
{
public:
	union
	{
		struct
		{
			float x,y,z,w;
		};
/*
		struct
		{
			uint64 xy, zw;
		};
*/
		uint128 packed;
	};

	static const MFVector zero;
	static const MFVector one;
	static const MFVector identity;
	static const MFVector up;

	// general purpose vector operators
	inline MFVector operator-();

	inline bool operator==(const MFVector &v) const;
	inline bool operator!=(const MFVector &v) const;

	inline MFVector& operator=(const MFVector &v);
	inline MFVector& operator+=(const MFVector &v);
	inline MFVector& operator-=(const MFVector &v);
	inline MFVector& operator*=(float f);
	inline MFVector& operator*=(const MFVector &v);
//	inline MFVector& operator/=(float f);			// Since division operators shoudl NEVER really be used anyway, i wont expose them..
//	inline MFVector& operator/=(const MFVector &v);	// teach people to write code properly ;)

	inline MFVector operator+(const MFVector &v) const;
	inline MFVector operator-(const MFVector &v) const;
	inline MFVector operator*(float f) const;
	inline MFVector operator*(const MFVector &v) const;
//	inline MFVector operator/(float f) const;
//	inline MFVector operator/(const MFVector &v) const;

	// these are some functions for the fastest possible implementations on all platforms
	// NOTE: These functions do NOT preserve the 'w' component!
	inline MFVector& Add3(const MFVector &v1, const MFVector &v2);
	inline MFVector& Sub3(const MFVector &v1, const MFVector &v2);
	inline MFVector& Mul3(const MFVector &v1, float f);
	inline MFVector& Mul3(const MFVector &v1, const MFVector &v2);
	inline MFVector& Mad3(const MFVector &v1, float f, const MFVector &v3);
	inline MFVector& Mad3(const MFVector &v1, const MFVector &v2, const MFVector &v3);
//	inline MFVector& Div3(const MFVector &v1, float f);
//	inline MFVector& Div3(const MFVector &v1, const MFVector &v2);

	// NOTE: These functions do NOT preserve the 'z' or 'w' components!
	inline MFVector& Add2(const MFVector &v1, const MFVector &v2);
	inline MFVector& Sub2(const MFVector &v1, const MFVector &v2);
	inline MFVector& Mul2(const MFVector &v1, float f);
	inline MFVector& Mul2(const MFVector &v1, const MFVector &v2);
	inline MFVector& Mad2(const MFVector &v1, float f, const MFVector &v3);
	inline MFVector& Mad2(const MFVector &v1, const MFVector &v2, const MFVector &v3);
//	inline MFVector& Div2(const MFVector &v1, float f);
//	inline MFVector& Div2(const MFVector &v1, const MFVector &v2);

	inline operator float*();
	inline operator float*() const;

	inline uint32 ToPackedColour() const;			// WARNING: this function produces different results on all hardware
	inline MFVector& FromPackedColour(uint32 col);

	inline float Dot4(const MFVector &vec) const;
	inline float DotH(const MFVector &vec4) const;
	inline float Dot3(const MFVector &vec) const;
	inline float Dot2(const MFVector &vec) const;

//	inline MFVector Cross4(const MFVector &vec) const;
	inline MFVector Cross3(const MFVector &vec) const;
	inline float Cross2(const MFVector &vec) const;

//	inline MFVector& Cross4(const MFVector &vec, const MFVector &vec2);
	inline MFVector& Cross3(const MFVector &vec, const MFVector &vec2);

/*
	inline MFVector& ApplyMatrix(const Matrix &matrix);
	inline MFVector& ApplyMatrixH(const Matrix &matrix);
	inline MFVector& ApplyMatrix3x3(const Matrix &matrix);
*/

	inline float MagSquared4() const;
	inline float MagSquared3() const;
	inline float MagSquared2() const;
	inline float Magnitude4() const;
	inline float Magnitude3() const;
	inline float Magnitude2() const;
	inline MFVector& Normalise4();
	inline MFVector& Normalise3();
	inline MFVector& Normalise2();

	inline float Distance(const MFVector &v) const;
	inline float GetAngle(const MFVector &ref = up);

	inline MFVector& Lerp(const MFVector &v, float t);

	inline char* ToString4() const;
	inline char* ToString3() const;
	inline char* ToString2() const;
};

inline MFVector operator*(float f, const MFVector &v);

inline MFVector MakeVector(float f);
inline MFVector MakeVector(float x, float y, float z = 0.0f, float w = 1.0f);
inline MFVector MakeVector(const MFVector &v, float w);

inline MFVector Min(const MFVector &a, const MFVector &b);
inline MFVector Max(const MFVector &a, const MFVector &b);

#include "MFVector.inl"

#endif
