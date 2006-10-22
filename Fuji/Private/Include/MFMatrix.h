/**
 * @file MFMatrix.h
 * @brief Describes the Mount Fuji Engine matrix class.
 * @author Manu Evans
 * @defgroup MFMatrix Matrix math Class
 * @{
 */

#if !defined(_MFMATRIX_H)
#define _MFMATRIX_H

#include "MFVector.h"
#include "MFQuaternion.h"

MFALIGN_BEGIN(16)
class MFMatrix
{
public:
	float m[16];

	static const MFMatrix identity;

	MFMatrix& operator=(const MFMatrix& mat);
	bool operator==(const MFMatrix& mat) const;
	bool operator!=(const MFMatrix& mat) const;

	MFMatrix& SetIdentity();

	MFMatrix& Translate(const MFVector &trans);
	MFMatrix& Rotate(const MFVector &axis, float angle);
	MFMatrix& RotateQ(const MFQuaternion &q);
	MFMatrix& RotateYPR(float yaw, float pitch, float roll);
	MFMatrix& RotateX(float angle);
	MFMatrix& RotateY(float angle);
	MFMatrix& RotateZ(float angle);
	MFMatrix& Scale(const MFVector& scale);

	MFMatrix& SetTranslation(const MFVector &trans);
	MFMatrix& SetRotation(const MFVector &axis, float angle);
	MFMatrix& SetRotationQ(const MFQuaternion &q);
	MFMatrix& SetRotationYPR(float yaw, float pitch, float roll);
	MFMatrix& SetRotationX(float angle);
	MFMatrix& SetRotationY(float angle);
	MFMatrix& SetRotationZ(float angle);
	MFMatrix& SetScale(const MFVector& scale);

	MFQuaternion GetRotationQ() const;

	MFMatrix& LookAt(const MFVector& pos, const MFVector& at, const MFVector& up = MFVector::up);

	MFMatrix& Transpose();
	MFMatrix& Transpose(const MFMatrix &matrix);
	MFMatrix& Transpose3x3();
	MFMatrix& Transpose3x3(const MFMatrix &matrix);

	MFMatrix& Multiply(const MFMatrix& mat);
	MFMatrix& Multiply(const MFMatrix& mat1, const MFMatrix& mat2);
	MFMatrix& Multiply4x4(const MFMatrix& mat);
	MFMatrix& Multiply4x4(const MFMatrix& mat1, const MFMatrix& mat2);
	MFMatrix& Multiply3x3(const MFMatrix& mat);
	MFMatrix& Multiply3x3(const MFMatrix& mat1, const MFMatrix& mat2);

	MFVector TransformVector(const MFVector& vec) const;
	MFVector TransformVectorH(const MFVector& vec) const;
	MFVector TransformVector3(const MFVector& vec) const;

	MFMatrix& Inverse() { return Inverse(*this); }
	MFMatrix& Inverse(const MFMatrix &matrix);

	MFMatrix& Tween(const MFMatrix& start, const MFMatrix& end, float t);
	MFMatrix& PreciseTween(const MFMatrix& start, const MFMatrix& end, float t);

	MFMatrix& ClearW();

	const MFVector& GetXAxis() const;
	const MFVector& GetYAxis() const;
	const MFVector& GetZAxis() const;
	const MFVector& GetTrans() const;

	void SetXAxis3(const MFVector &v);
	void SetYAxis3(const MFVector &v);
	void SetZAxis3(const MFVector &v);
	void SetTrans3(const MFVector &v);
	void SetXAxis4(const MFVector &v);
	void SetYAxis4(const MFVector &v);
	void SetZAxis4(const MFVector &v);
	void SetTrans4(const MFVector &v);

	const char * ToString() const;
}
MFALIGN_END(16);

MFVector ApplyMatrix(const MFVector& vector, const MFMatrix &matrix);
MFVector ApplyMatrixH(const MFVector& vector, const MFMatrix &matrix);
MFVector ApplyMatrix3(const MFVector& vector, const MFMatrix &matrix);

#include "MFMatrix.inl"

// this is to make doxygen happy...
#if 0

/**
 * Matrix class to perform various matrix operations.
 * Matrix class used throughout Fuji. Optimised for each platform.
 */
class MFMatrix
{
public:
	float m[16]; /**< 4x4 array of floats */

	static const MFMatrix identity;	/**< Constant identity matrix */


	MFMatrix& operator=(const MFMatrix& mat);	/**< Assignment operator. */
	bool operator==(const MFMatrix& mat) const;	/**< Comparison operator. */
	bool operator!=(const MFMatrix& mat) const;	/**< Not-equals operator. */

	MFMatrix& SetIdentity();	/**< Set the matrix to identity */

	MFMatrix& Translate(const MFVector &trans);					/**< Translates the matrix */
	MFMatrix& Rotate(const MFVector &axis, float angle);		/**< Rotates the matrix about an axis */
	MFMatrix& RotateQ(const MFVector &q);						/**< Rotates the matrix by a quaternion */
	MFMatrix& RotateYPR(float yaw, float pitch, float roll);	/**< Rotates the matrix arbitrarily */
	MFMatrix& RotateX(float angle);			/**< Rotates the matrix about the X axis */
	MFMatrix& RotateY(float angle);			/**< Rotates the matrix about the Y axis */
	MFMatrix& RotateZ(float angle);			/**< Rotates the matrix about the Z axis */
	MFMatrix& Scale(const MFVector& scale);	/**< Scales the matrix */

	MFMatrix& SetTranslation(const MFVector &trans);				/**< Set the matrix translation */
	MFMatrix& SetRotation(const MFVector &axis, float angle);		/**< Set the matrix rotation */
	MFMatrix& SetRotationQ(const MFQuaternion &q);						/**< Set the matrix rotation to a quaternion */
	MFMatrix& SetRotationYPR(float yaw, float pitch, float roll);	/**< Set the matrix rotation to a specified pitch yaw and roll */
	MFMatrix& SetRotationX(float angle);		/**< Sets the rotation to rotate about the X axis */
	MFMatrix& SetRotationY(float angle);		/**< Sets the rotation to rotate about the Y axis */
	MFMatrix& SetRotationZ(float angle);		/**< Sets the rotation to rotate about the Z axis */
	MFMatrix& SetScale(const MFVector& scale);	/**< Sets the scale of the matrix */

	MFQuaternion GetRotationQ() const;				/**< Calculate a quaternion from the matrix */

	MFMatrix& LookAt(const MFVector& pos, const MFVector& at, const MFVector& up = MFVector::up);	/**< Build a matrix that looks from one point to another */

	MFMatrix& Transpose();							/**< Transpose the matrix */
	MFMatrix& Transpose(const MFMatrix &matrix);	/**< Sets the matrix to the transpose of another */
	MFMatrix& Transpose3x3();						/**< Transpose the 3x3 portion of the matrix */
	MFMatrix& Transpose3x3(const MFMatrix &matrix);	/**< Sets the matrix to the transpose of another 3x3 matrix */

	MFMatrix& Multiply(const MFMatrix& mat);							/**< Multiply the matrix by another */
	MFMatrix& Multiply(const MFMatrix& mat1, const MFMatrix& mat2);		/**< Set the matrix to the product of 2 matrices */
	MFMatrix& Multiply4x4(const MFMatrix& mat);							/**< Multiply the matrix by another full 4x4 matrix */
	MFMatrix& Multiply4x4(const MFMatrix& mat1, const MFMatrix& mat2);	/**< Set the matrix to the product of 2 full 4x4 matrices */
	MFMatrix& Multiply3x3(const MFMatrix& mat);							/**< Multiply the 3x3 matrix by another 3x3 matrix */
	MFMatrix& Multiply3x3(const MFMatrix& mat1, const MFMatrix& mat2);	/**< Set the matrix to the product of 2 3x3 matrices */

	MFVector TransformVector(const MFVector& vec) const;	/**< Transform a vector by the matrix */
	MFVector TransformVectorH(const MFVector& vec) const;	/**< Transform a vector by the matrix assuming 1.0 in the vector's W component */
	MFVector TransformVector3(const MFVector& vec) const;	/**< Transform a 3D vector by a rotation matrix */

	MFMatrix& Inverse() { return Inverse(*this); }	/**< Inverse the matrix */
	MFMatrix& Inverse(const MFMatrix &matrix);		/**< Set this matrix to the inverse of another matrix */

	MFMatrix& Tween(const MFMatrix& start, const MFMatrix& end, float t);			/**< Tween one matrix towards another by a factor \a t */
	MFMatrix& PreciseTween(const MFMatrix& start, const MFMatrix& end, float t);	/**< Tween one matrix towards another by a factor \a t using a quaternion to interpolate rotation */

	MFMatrix& ClearW();	/**< Clear the matrix W column */

	const MFVector& GetXAxis() const;	/**< Get the matrices X axis */
	const MFVector& GetYAxis() const;	/**< Get the matrices Y axis */
	const MFVector& GetZAxis() const;	/**< Get the matrices Z axis */
	const MFVector& GetTrans() const;	/**< Get the matrices translation vector */

	void SetXAxis3(const MFVector &v);	/**< Set the matrices X axis to a 3D vector */
	void SetYAxis3(const MFVector &v);	/**< Set the matrices Y axis to a 3D vector */
	void SetZAxis3(const MFVector &v);	/**< Set the matrices Z axis to a 3D vector */
	void SetTrans3(const MFVector &v);	/**< Set the matrices translation to a 3D vector */
	void SetXAxis4(const MFVector &v);	/**< Set the matrices X axis to a 4D vector */
	void SetYAxis4(const MFVector &v);	/**< Set the matrices Y axis to a 4D vector */
	void SetZAxis4(const MFVector &v);	/**< Set the matrices Z axis to a 4D vector */
	void SetTrans4(const MFVector &v);	/**< Set the matrices translation to a 4D vector */

	const char * ToString() const;		/**< Generate a string that represents the matrix */
};

/**
 * Transform a vector by the matrix
 */
MFVector ApplyMatrix(const MFVector& vector, const MFMatrix &matrix);

/**
 * Transform a vector by the matrix
 */
MFVector ApplyMatrixH(const MFVector& vector, const MFMatrix &matrix);

/**
 * Transform a vector by the matrix
 */
MFVector ApplyMatrix3(const MFVector& vector, const MFMatrix &matrix);

#endif // make doxygen happy

#endif // _MFMATRIX_H

/** @} */
