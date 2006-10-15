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

enum MFSwizzleFlags
{
	SW_X = 0,
	SW_Y,
	SW_Z,
	SW_W,
	SW_ComponentMask = 0x3,

	SW_NEG = 4,
	SW_ABS = 8
};

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

	static const MFVector red;
	static const MFVector green;
	static const MFVector blue;
	static const MFVector yellow;
	static const MFVector white;
	static const MFVector black;

	// general purpose vector operators

	void Set(float x, float y, float z = 0.0f, float w = 1.0f);
	void Swizzle(const MFVector &source, const uint8 x, const uint8 y, const uint8 z, const uint8 w);

	bool operator==(const MFVector &v) const;
	bool operator!=(const MFVector &v) const;

	MFVector operator-() const;

	MFVector& operator=(const MFVector &v);
	MFVector& operator+=(const MFVector &v);
	MFVector& operator-=(const MFVector &v);
	MFVector& operator*=(float f);
	MFVector& operator*=(const MFVector &v);
//	MFVector& operator/=(float f);				// Since division operators should never really be used anyway, i wont expose them..
//	MFVector& operator/=(const MFVector &v);	// might help get people in the habit of using reciprocals.. :P

	MFVector operator+(const MFVector &v) const;
	MFVector operator-(const MFVector &v) const;
	MFVector operator*(float f) const;
	MFVector operator*(const MFVector &v) const;
//	MFVector operator/(float f) const;
//	MFVector operator/(const MFVector &v) const;

	// these are some functions for the fastest possible implementations on all platforms
	MFVector& Add4(const MFVector &v1, const MFVector &v2);
	MFVector& Sub4(const MFVector &v1, const MFVector &v2);
	MFVector& Mul4(const MFVector &v1, float f);
	MFVector& Mul4(const MFVector &v1, const MFVector &v2);
	MFVector& Mad4(const MFVector &v1, float f, const MFVector &v3);
	MFVector& Mad4(const MFVector &v1, const MFVector &v2, const MFVector &v3);
//	MFVector& Div4(const MFVector &v1, float f);
//	MFVector& Div4(const MFVector &v1, const MFVector &v2);

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
	operator const float*() const;

	uint32 ToPackedColour() const;			// WARNING: this function produces different results on all hardware. Packs to the hardwares native 32bit colour format.
	MFVector& FromPackedColour(uint32 col);

	MFVector& Rcp4(const MFVector &v);
	MFVector& Rcp3(const MFVector &v);
	MFVector& Rcp2(const MFVector &v);

	float Dot4(const MFVector &vec) const;
	float DotH(const MFVector &vec4) const;
	float Dot3(const MFVector &vec) const;
	float Dot2(const MFVector &vec) const;

//	MFVector Cross4(const MFVector &vec) const;
	MFVector Cross3(const MFVector &vec) const;
	float Cross2(const MFVector &vec) const;

//	MFVector& Cross4(const MFVector &vec, const MFVector &vec2);
	MFVector& Cross3(const MFVector &vec, const MFVector &vec2);

	float MagSquared4() const;
	float MagSquared3() const;
	float MagSquared2() const;
	float Magnitude4() const;
	float Magnitude3() const;
	float Magnitude2() const;
	float InvMagnitude4() const;
	float InvMagnitude3() const;
	float InvMagnitude2() const;
	MFVector& Normalise4();
	MFVector& Normalise3();
	MFVector& Normalise2();
	MFVector& Normalise4(const MFVector &vec);
	MFVector& Normalise3(const MFVector &vec);
	MFVector& Normalise2(const MFVector &vec);

	float Distance(const MFVector &v) const;
	float GetAngle(const MFVector &ref = up) const;

	MFVector& Lerp(const MFVector &v, float t);

	const char * ToString4() const;
	const char * ToString3() const;
	const char * ToString2() const;
}
MFALIGN_END(16);

MFVector operator*(float f, const MFVector &v);

MFVector MakeVector(float f);
MFVector MakeVector(float x, float y, float z = 0.0f, float w = 1.0f);
MFVector MakeVector(const MFVector &v, float w);

MFVector MFAbs(const MFVector &v);
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

	static const MFVector red;		/**< Constant RED colour vector { 1, 0, 0, 1 } */
	static const MFVector green;	/**< Constant GREEN colour vector { 0, 1, 0, 1 } */
	static const MFVector blue;		/**< Constant BLUE colour vector { 0, 0, 1, 1 } */
	static const MFVector yellow;	/**< Constant YELLOW colour vector { 1, 1, 0, 1 } */
	static const MFVector white;	/**< Constant WHITE colour vector { 1, 1, 1, 1 } */
	static const MFVector black;	/**< Constant BLACK colour vector { 0, 0, 0, 1 } */


	// general purpose vector operators

	void Set(float x, float y, float z = 0.0f, float w = 1.0f);	/**< Set each component of a vector. */
	void Swizzle(const MFVector &source, const uint8 x, const uint8 y, const uint8 z, const uint8 w);	/**< Swizzle components within a vector. */

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
	MFVector operator*(float f) const;			 /**< Scale operator. */
	MFVector operator*(const MFVector &v) const; /**< Modulate operator. */

	MFVector& Add4(const MFVector &v1, const MFVector &v2);						/**< Add 4D function. */
	MFVector& Sub4(const MFVector &v1, const MFVector &v2);						/**< Subtract 4D function. */
	MFVector& Mul4(const MFVector &v1, float f);								/**< Scale 4D function. */
	MFVector& Mul4(const MFVector &v1, const MFVector &v2);						/**< Multiply 4D function. */
	MFVector& Mad4(const MFVector &v1, float f, const MFVector &v3);			/**< Scale and Add 4D function. */
	MFVector& Mad4(const MFVector &v1, const MFVector &v2, const MFVector &v3);	/**< Multiply and Add 4D function. */

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

	MFVector& Rcp4(const MFVector &v);	/**< Find the reciprocal of all 4 components. */
	MFVector& Rcp3(const MFVector &v);	/**< Find the reciprocal of the first 3 components. */
	MFVector& Rcp2(const MFVector &v);	/**< Find the reciprocal of the first 2 components. */

	float Dot4(const MFVector &vec) const;	/**< Dot Product 4D. */
	float DotH(const MFVector &vec4) const;	/**< Dot Product Homogeneous (assumes this vectors w = 1). */
	float Dot3(const MFVector &vec) const;	/**< Dot Product 3D. */
	float Dot2(const MFVector &vec) const;	/**< Dot Product 2D. */

	MFVector Cross3(const MFVector &vec) const;	/**< Cross Product 3D. */
	float Cross2(const MFVector &vec) const;	/**< Cross Product 2D. */

	MFVector& Cross3(const MFVector &vec, const MFVector &vec2);	/**< Cross Product 3D. */

	float MagSquared4() const;		/**< Get the 4D Magnitude Squared. */
	float MagSquared3() const;		/**< Get the 3D Magnitude Squared. */
	float MagSquared2() const;		/**< Get the 2D Magnitude Squared. */
	float Magnitude4() const;		/**< Get the 4D Magnitude. */
	float Magnitude3() const;		/**< Get the 3D Magnitude. */
	float Magnitude2() const;		/**< Get the 2D Magnitude. */
	float InvMagnitude4() const;	/**< Get the inverse 4D Magnitude. */
	float InvMagnitude3() const;	/**< Get the inverse 3D Magnitude. */
	float InvMagnitude2() const;	/**< Get the inverse 2D Magnitude. */
	MFVector& Normalise4();			/**< Normalise vector 4D. */
	MFVector& Normalise3();			/**< Normalise vector 3D. */
	MFVector& Normalise2();			/**< Normalise vector 2D. */
	MFVector& Normalise4(const MFVector &vec);	/**< Normalise vector 4D. */
	MFVector& Normalise3(const MFVector &vec);	/**< Normalise vector 3D. */
	MFVector& Normalise2(const MFVector &vec);	/**< Normalise vector 2D. */

	float Distance(const MFVector &v) const;	/**< Get the distance between 2 vectors. */
	float GetAngle(const MFVector &ref = up);	/**< Get the angle between 2 vectors. */

	MFVector& Lerp(const MFVector &v, float t);	/**< Lerp between 2 vectors. */

	const char * ToString4() const;	/**< Convert 4D vector to a string. */
	const char * ToString3() const;	/**< Convert 3D vector to a string. */
	const char * ToString2() const;	/**< Convert 2D vector to a string. */
};

#endif // make doxygen happy

#endif // _MFVECTOR_H

/** @} */
