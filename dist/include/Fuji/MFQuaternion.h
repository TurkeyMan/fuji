/**
 * @file MFQuaternion.h
 * @brief Describes the Mount Fuji Engine quaternion class.
 * @author Manu Evans
 * @defgroup MFQuaternion Quaternion math Class
 * @{
 */

#pragma once
#if !defined(_MFQUATERNION_H)
#define _MFQUATERNION_H

#include "MFVector.h"

#if defined(_MSC_VER)
#pragma warning(disable:4201)
#endif

MFALIGN_BEGIN(16)
class MFQuaternion
{
public:
	struct
	{
		float x, y, z, w;
	};

	static const MFQuaternion identity;

	// general purpose quaternion operators
	bool operator==(const MFQuaternion &q) const;
	bool operator!=(const MFQuaternion &q) const;

	void SetIdentity();

	void Set(float x, float y, float z, float w);

	MFQuaternion& operator=(const MFQuaternion &q);

	MFQuaternion operator+(const MFQuaternion &q) const;
	MFQuaternion operator-(const MFQuaternion &q) const;
	MFQuaternion operator*(float f) const;
	MFQuaternion operator*(const MFQuaternion &q) const;

	MFQuaternion& operator+=(const MFQuaternion &q);
	MFQuaternion& operator-=(const MFQuaternion &q);
	MFQuaternion& operator*=(float f);
	MFQuaternion& operator*=(const MFQuaternion &q);

	float Dot(const MFQuaternion &q) const;

	MFQuaternion& Multiply(const MFQuaternion &q1, const MFQuaternion &q2);
	MFQuaternion& Multiply(const MFQuaternion &q);

	MFQuaternion& Conjugate(const MFQuaternion &q);
	MFQuaternion& Conjugate();

	MFVector Apply(const MFVector &v) const;

	operator float*();
	operator const float*() const;

	MFQuaternion& Slerp(const MFQuaternion &q1, const MFQuaternion &q2, float t);
	MFQuaternion& Slerp(const MFQuaternion &q, float t);

	const char * ToString() const;
}
MFALIGN_END(16);

MFQuaternion MakeQuat(float x, float y, float z, float w);

#include "MFQuaternion.inl"

// this is to make doxygen happy...
#if 0

/**
 * Quaternion class to perform various quaternion operations.
 * Quaternion class used throughout Fuji. Optimised for each platform.
 */
class MFQuaternion
{
public:
	struct
	{
		float x; /**< X imaginary component */
		float y; /**< Y imaginary component */
		float z; /**< Z imaginary component */
		float w; /**< W real component */
	};

	static const MFQuaternion identity;							/**< Constant IDENTITY quaternion { 0, 0, 0, 1 } */


	// general purpose vector operators

	bool operator==(const MFQuaternion &q) const;				/**< Comparison operator. */
	bool operator!=(const MFQuaternion &q) const;				/**< Not-equals operator. */

	void SetIdentity();											/**< Set quaternion to the identity quaternion. */

	void Set(float x, float y, float z, float w);				/**< Set each component of a quaternion. */

	MFQuaternion& operator=(const MFQuaternion &v);				/**< Assignment operator. */

	MFQuaternion operator+(const MFQuaternion &q) const;		/**< Addition operator. */
	MFQuaternion operator-(const MFQuaternion &q) const;		/**< Subtraction operator. */
	MFQuaternion operator*(float f) const;						/**< Scale operator. */
	MFQuaternion operator*(const MFQuaternion &q) const;		/**< Quaternion multiply operator. */

	MFQuaternion& operator+=(const MFQuaternion &q);			/**< Plus-equals operator. */
	MFQuaternion& operator-=(const MFQuaternion &q);			/**< Minus-equals operator. */
	MFQuaternion& operator*=(float f);							/**< Scale-equals operator. */
	MFQuaternion& operator*=(const MFQuaternion &q);			/**< Quaternion multiply-equals operator. */

	float Dot(const MFQuaternion &q) const;						/**< Quaternion dot product. */

	MFQuaternion& Multiply(const MFQuaternion &q);				/**< Perform a quaternion multiply. */
	MFQuaternion& Multiply(const MFQuaternion &q1, const MFQuaternion &q2);	/**< Perform a quaternion multiply. */

	MFQuaternion& Conjugate(const MFQuaternion &q);				/**< Find the conjugate of a quaternion. */
	MFQuaternion& Conjugate();									/**< Calculate the quaternion conjugate. */

	MFVector Apply(const MFVector &v) const;					/**< Apply the quaternion to a vector. */

	operator float*();											/**< float pointer cast operator. */
	operator float*() const;									/**< const float pointer cast operator. */

	MFQuaternion& Slerp(const MFQuaternion &q1, const MFQuaternion &q2, float t);	/**< Slerp between 2 quaternions. */
	MFQuaternion& Slerp(const MFQuaternion &q, float t);		/**< Slerp between this and another quaternion. */

	const char * ToString() const;								/**< Convert quaternion to a string. */
};

#endif // make doxygen happy

#endif // _MFQUATERNION_H

/** @} */
