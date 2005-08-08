#if !defined(_MFVECTOR_H)
#define _MFVECTOR_H

#if defined(_MSC_VER)
#pragma warning(disable:4201)
#endif

class Matrix;

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

	inline MFVector operator-();

	inline bool operator==(const MFVector &v) const;
	inline bool operator!=(const MFVector &v) const;

	inline MFVector& operator=(const MFVector &v);
	inline MFVector& operator+=(const MFVector &v);
	inline MFVector& operator-=(const MFVector &v);
	inline MFVector& operator*=(float f);
	inline MFVector& operator*=(const MFVector &v);
//	inline MFVector& operator/=(float f);
//	inline MFVector& operator/=(const MFVector &v);

	inline MFVector operator+(const MFVector &v) const;
	inline MFVector operator-(const MFVector &v) const;
	inline MFVector operator*(float f) const;
	inline MFVector operator*(const MFVector &v) const;
//	inline MFVector operator/(float f) const;
//	inline MFVector operator/(const MFVector &v) const;

	inline operator float*();
	inline operator float*() const;

	inline uint32 ToPackedColour() const;
	inline MFVector& FromPackedColour(uint32 col);

	inline float Dot4(const MFVector &vec) const;
	inline float DotH(const MFVector &vec4) const;
	inline float Dot3(const MFVector &vec) const;
	inline float Dot2(const MFVector &vec) const;

//	inline MFVector Cross4(const MFVector &vec) const;
	inline MFVector Cross(const MFVector &vec) const;
	inline float Cross2(const MFVector &vec) const;

//	inline MFVector& Cross4(const MFVector &vec, const MFVector &vec2);
	inline MFVector& Cross(const MFVector &vec, const MFVector &vec2);

	inline MFVector& ApplyMatrix(const Matrix &matrix);
	inline MFVector& ApplyMatrixH(const Matrix &matrix);
	inline MFVector& ApplyMatrix3x3(const Matrix &matrix);

	inline float MagSquared4() const;
	inline float MagSquared() const;
	inline float MagSquared2() const;
	inline float Magnitude4() const;
	inline float Magnitude() const;
	inline float Magnitude2() const;
	inline MFVector& Normalise4();
	inline MFVector& Normalise();
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
