/**
 * @file MFMath.h
 * @brief Declares some Fuji math functions, optimised for each platform.
 * @author Manu Evans
 * @defgroup MFMath Math Functions
 * @{
 */

#if !defined(_MFMATH_H)
#define _MFMATH_H

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
 * @return Returns \a x raised to the power of \a y.
 * @see MFSqrt()
 */
float MFPow(float x, float y);

/**
 * Returns the next integer greater than or equal to \a x.
 * Returns the next integer greater than or equal to \a x.
 * @param x Value to find the ceil.
 * @return Returns the next integer greater than or equal to \a x.
 * @see MFFloor()
 */
float MFCeil(float x);

/**
 * Returns the next integer less than or equal to \a x.
 * Returns the next integer less than or equal to \a x.
 * @param x Value to find the floor.
 * @return Returns the next integer less than or equal to \a x.
 * @see MFCeil()
 */
float MFFloor(float x);

/**
 * Calculates the absolute of a value.
 * Calculates the absolute of a value.
 * @return The absolute of \a x.
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
template <typename T>
T MFMin(T a, T b);
/**
 * Returns the maximum of \a a and \a b.
 * Returns the maximum of \a a and \a b.
 * @see MFMin()
 * @see MFClamp()
 */
template <typename T>
T MFMax(T a, T b);

/**
 * Clamps values such that: \a x <= \a y <= \a z.
 * Clamps values such that: \a x <= \a y <= \a z.
 * @see MFMax()
 * @see MFMin()
 */
template <typename T>
T MFClamp(T x, T y, T z);

/**
 * Seed the random number generator.
 * Seeds the random number generator.
 * @return None.
 * @see MFRand()
 * @see MFRand_Unit()
 */
void MFRand_Seed(uint32 seed);

/**
 * Get a pseudo-random number.
 * Get a pseudo-random number.
 * @return Returns a pseudo random number in the range of 0 to 2^32-1 (4294967295).
 * @see MFRand_Unit()
 * @see MFRand_Range()
 */
uint32 MFRand();

/**
 * Get a pseudo-random number between 0.0f and 1.0f.
 * Get a pseudo-random number between 0.0f and 1.0f.
 * @return Returns a pseudo-random number betwen 0.0f and 1.0f.
 * @see MFRand()
 * @see MFRand_Range()
 */
float MFRand_Unit();

/**
 * Get a pseudo-random double precision number between 0.0f and 1.0f.
 * Get a pseudo-random double precision number between 0.0f and 1.0f.
 * @return Returns a double precision pseudo-random number betwen 0.0f and 1.0f.
 * @see MFRand()
 * @see MFRand_Unit()
 */
double MFRand_Double();

/**
 * Get a pseudo-random number within a specified range.
 * Get a pseudo-random number within a specified range.
 * @param min Minimum value.
 * @param max Maximum value.
 * @return Returns a pseudo-random number between \a min and \a max.
 * @see MFRand()
 * @see MFRand_Unit()
 */
float MFRand_Range(float min, float max);

#include "MFMath.inl"

#endif

/** @} */
