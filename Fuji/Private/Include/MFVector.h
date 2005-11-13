/**
 * @file MFVector.h
 * @brief Describes the Mount Fuji Engine vector class.
 * @author Manu Evans
 * @defgroup MFVector Vector math Class
 * @{
 */

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
		float x, y, z, w;
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

/**
 * Generates a random vector.
 * Generates a random vector within a unit cube.
 * @return A random vector within a unit cube.
 * @see MFRand()
 * @see MFRand_Unit()
 * @see MFRand_Range()
 */
MFVector MFRand_Vector();

#include "MFVector.inl"

// this is to make doxygen happy...
#if 0

/**
 * Vector class to perform various vector operations.
 * Vector class used throughout Fuji. Optimised for each platform.
 */
class MFVector
{
public:
	struct
	{
		float x; /**< X component */
		float y; /**< Y component */
		float z; /**< Z component */
		float w; /**< W component */
	};

	static const MFVector zero;		/**< Constant ZERO vector { 0, 0, 0, 0 } */
	static const MFVector one;		/**< Constant ONE vector { 1, 1, 1, 1 } */
	static const MFVector identity;	/**< Constant IDENTITY vector { 0, 0, 0, 1 } */
	static const MFVector up;		/**< Constant UP vector { 0, 1, 0, 1 } */

	// general purpose vector operators

	MFVector operator-(); /**< Unary minus operator. */

	bool operator==(const MFVector &v) const;	/**< Comparison operator. */
	bool operator!=(const MFVector &v) const;	/**< Not-equals operator. */

	MFVector& operator=(const MFVector &v);		/**< Equals operator. */
	MFVector& operator+=(const MFVector &v);	/**< Plus-equals operator. */
	MFVector& operator-=(const MFVector &v);	/**< Minus-equals operator. */
	MFVector& operator*=(float f);				/**< Scale-equals operator. */
	MFVector& operator*=(const MFVector &v);	/**< Modulate-equals operator. */

	MFVector operator+(const MFVector &v) const; /**< Addition operator. */
	MFVector operator-(const MFVector &v) const; /**< Subtraction operator. */
	MFVector operator*(float f) const; /**< Scale operator. */
	MFVector operator*(const MFVector &v) const; /**< Modulate operator. */

	MFVector& Add3(const MFVector &v1, const MFVector &v2);						/**< Add 3D function. */
	MFVector& Sub3(const MFVector &v1, const MFVector &v2);						/**< Subtract 3D function. */
	MFVector& Mul3(const MFVector &v1, float f);								/**< Scale 3D function. */
	MFVector& Mul3(const MFVector &v1, const MFVector &v2);						/**< Multiply 3D function. */
	MFVector& Mad3(const MFVector &v1, float f, const MFVector &v3);			/**< Scale and Add 3D function. */
	MFVector& Mad3(const MFVector &v1, const MFVector &v2, const MFVector &v3);	/**< Multiply and Add 3D function. */

	MFVector& Add2(const MFVector &v1, const MFVector &v2);						/**< Add 2D function. */
	MFVector& Sub2(const MFVector &v1, const MFVector &v2);						/**< Subtract 2D function. */
	MFVector& Mul2(const MFVector &v1, float f);								/**< Scale 2D function. */
	MFVector& Mul2(const MFVector &v1, const MFVector &v2);						/**< Multiply 2D function. */
	MFVector& Mad2(const MFVector &v1, float f, const MFVector &v3);			/**< Scale and Add 2D function. */
	MFVector& Mad2(const MFVector &v1, const MFVector &v2, const MFVector &v3);	/**< Multiply and Add 2D function. */

	operator float*();			/**< float pointer cast operator. */
	operator float*() const;	/**< const float pointer cast operator. */

	uint32 ToPackedColour() const;			/**< Get platform-specific packed colour. */
	MFVector& FromPackedColour(uint32 col);	/**< Build vector from platform-specific packed colour. */

	float Dot4(const MFVector &vec) const;	/**< Dot Product 4D. */
	float DotH(const MFVector &vec4) const;	/**< Dot Product Homogeneous (assumes this vectors w = 1). */
	float Dot3(const MFVector &vec) const;	/**< Dot Product 3D. */
	float Dot2(const MFVector &vec) const;	/**< Dot Product 2D. */

	MFVector Cross3(const MFVector &vec) const;	/**< Cross Product 3D. */
	float Cross2(const MFVector &vec) const;	/**< Cross Product 2D. */

	MFVector& Cross3(const MFVector &vec, const MFVector &vec2);	/**< Cross Product 3D. */

	float MagSquared4() const;	/**< Get the 4D Magnitude Squared. */
	float MagSquared3() const;	/**< Get the 3D Magnitude Squared. */
	float MagSquared2() const;	/**< Get the 2D Magnitude Squared. */
	float Magnitude4() const;	/**< Get the 4D Magnitude. */
	float Magnitude3() const;	/**< Get the 3D Magnitude. */
	float Magnitude2() const;	/**< Get the 2D Magnitude. */
	MFVector& Normalise4();		/**< Normalise vector 4D. */
	MFVector& Normalise3();		/**< Normalise vector 3D. */
	MFVector& Normalise2();		/**< Normalise vector 2D. */

	float Distance(const MFVector &v) const;	/**< Get the distance between 2 vectors. */
	float GetAngle(const MFVector &ref = up);	/**< Get the angle between 2 vectors. */

	MFVector& Lerp(const MFVector &v, float t);	/**< Lerp between 2 vectors. */

	char* ToString4() const;	/**< Convert 4D vector to a string. */
	char* ToString3() const;	/**< Convert 3D vector to a string. */
	char* ToString2() const;	/**< Convert 2D vector to a string. */
};

#endif // make doxygen happy

#endif // _MFVECTOR_H

/** @} */
