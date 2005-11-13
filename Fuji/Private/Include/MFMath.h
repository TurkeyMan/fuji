/**
 * @file MFMath.h
 * @brief Declares some Fuji math functions, optimised for each platform.
 * @author Manu Evans
 * @defgroup MFMath Math Functions
 * @{
 */

#if !defined(_FUJI_MATH)
#define _FUJI_MATH

/**
 * Calculates the sine of an angle.
 * Calculates the sine of an angle.
 * @param angle An angle in radians.
 * @return The sine of \a angle.
 * @see MFCos()
 * @see MFTan()
 */
float MFSin(float angle);

/**
 * Calculates the cosine of an angle.
 * Calculates the cosine of an angle.
 * @param angle An angle in radians.
 * @return The cosine of \a angle.
 * @see MFSin()
 * @see MFTan()
 */
float MFCos(float angle);

/**
 * Calculates the tangent of an angle.
 * Calculates the tangent of an angle.
 * @param angle An angle in radians.
 * @return The tangent of \a angle.
 * @see MFSin()
 * @see MFCos()
 */
float MFTan(float angle);

/**
 * Calculates the arc-sine of an angle.
 * Calculates the arc-sine of an angle.
 * @param angle An angle in radians.
 * @return The arc-sine of \a angle.
 * @see MFACos()
 * @see MFATan()
 */
float MFASin(float angle);

/**
 * Calculates the arc-cosine of an angle.
 * Calculates the arc-cosine of an angle.
 * @param angle An angle in radians.
 * @return The arc-cosine of \a angle.
 * @see MFASin()
 * @see MFATan()
 */
float MFACos(float angle);

/**
 * Calculates the arc-tangent of an angle.
 * Calculates the arc-tangent of an angle.
 * @param angle An angle in radians.
 * @return The arc-tangent of \a angle.
 * @see MFASin()
 * @see MFACos()
 */
float MFATan(float angle);

/**
 * Calculates the reciprocal of a value.
 * Calculates the reciprocal of a value.
 * @param x Any non-zero value.
 * @return The reciprocal of \a x.
 * @see MFRSqrt()
 */
float MFRcp(float x);

/**
 * Calculates the square root of a value.
 * Calculates the square root of a value.
 * @param x Any positive value.
 * @return The square root of \a x.
 * @see MFRSqrt()
 */
float MFSqrt(float x);

/**
 * Calculates the reciprocal square root of a value.
 * Calculates the reciprocal square root of a value.
 * @param x Any positive value.
 * @return The reciprocal square root of \a x.
 * @see MFSqrt()
 */
float MFRSqrt(float x);

/**
 * Raises a value to an arbitrary power.
 * Raises a value to an arbitrary power.
 * @param x The base value.
 * @param y The power to raise.
 * @return Returns /a x raised to the power of /a y.
 * @see MFSqrt()
 */
float MFPow(float x, float y);

/**
 * Calculates the absolute of a value.
 * Calculates the absolute of a value.
 * @return The absolute of /a x.
 * @see MFClamp()
 */
template <typename T>
T MFAbs(T x);

/**
 * Returns the minimum of \a a and \a b.
 * Returns the minimum of \a a and \a b.
 * @see MFMax()
 * @see MFClamp()
 */
template <class T>
T MFMin(T a, T b);
/**
 * Returns the maximum of \a a and \a b.
 * Returns the maximum of \a a and \a b.
 * @see MFMin()
 * @see MFClamp()
 */
template <class T>
T MFMax(T a, T b);

/**
 * Clamps values such that: \a x <= \a y <= \a z.
 * Clamps values such that: \a x <= \a y <= \a z.
 * @see MFMax()
 * @see MFMin()
 */
template <class T>
T MFClamp(T x, T y, T z);

#include "MFMath.inl"

#endif

/** @} */
