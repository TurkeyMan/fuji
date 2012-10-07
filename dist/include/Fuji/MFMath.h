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
 * Convert a string containing a hexadecimal number to an int.
 * Converts a string containing a hexadecimal number to an int.
 * @param pString String containing a hexadecimal number.
 * @return The value of the hexadecimal number in the string.
 */
int MFHexToInt(const char *pString);

/**
 * Calculates the sine of an angle.
 * Calculates the sine of an angle.
 * @param angle An angle in radians.
 * @return The sine of \a angle.
 * @see MFCos(), MFTan()
 */
float MFSin(float angle);

/**
 * Calculates the cosine of an angle.
 * Calculates the cosine of an angle.
 * @param angle An angle in radians.
 * @return The cosine of \a angle.
 * @see MFSin(), MFTan()
 */
float MFCos(float angle);

/**
 * Calculates the tangent of an angle.
 * Calculates the tangent of an angle.
 * @param angle An angle in radians.
 * @return The tangent of \a angle.
 * @see MFSin(), MFCos()
 */
float MFTan(float angle);

/**
 * Calculates the arc-sine of an angle.
 * Calculates the arc-sine of an angle.
 * @param angle An angle in radians.
 * @return The arc-sine of \a angle.
 * @see MFACos(), MFATan()
 */
float MFASin(float angle);

/**
 * Calculates the arc-cosine of an angle.
 * Calculates the arc-cosine of an angle.
 * @param angle An angle in radians.
 * @return The arc-cosine of \a angle.
 * @see MFASin(), MFATan()
 */
float MFACos(float angle);

/**
 * Calculates the arc-tangent of an angle.
 * Calculates the arc-tangent of an angle.
 * @param angle An angle in radians.
 * @return The arc-tangent of \a angle.
 * @see MFASin(), MFACos()
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
 * Very fast reciprocal square root estimate of a value.
 * Very fast reciprocal square root estimate of a value.
 * @param x Any positive value.
 * @return The reciprocal square root estimate of \a x.
 * @see MFSqrt(), MFRSqrt()
 */
float MFRSqrtE(float x);

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
 * @see MFMax(), MFClamp()
 */
template <typename T>
T MFMin(T a, T b);
/**
 * Returns the maximum of \a a and \a b.
 * Returns the maximum of \a a and \a b.
 * @see MFMin(), MFClamp()
 */
template <typename T>
T MFMax(T a, T b);

/**
 * Clamps values such that: \a x <= \a y <= \a z.
 * Clamps values such that: \a x <= \a y <= \a z.
 * @see MFMax(), MFMin()
 */
template <typename T>
T MFClamp(T x, T y, T z);

/**
 * Seed the random number generator.
 * Seeds the random number generator.
 * @return None.
 * @see MFRand(), MFRand_Unit()
 */
MF_API void MFRand_Seed(uint32 seed);

/**
 * Get a pseudo-random number.
 * Get a pseudo-random number.
 * @return Returns a pseudo random number in the range of 0 to 2^32-1 (4294967295).
 * @see MFRand_Unit(), MFRand_Range()
 */
MF_API uint32 MFRand();

/**
 * Get a pseudo-random number between 0.0f and 1.0f.
 * Get a pseudo-random number between 0.0f and 1.0f.
 * @return Returns a pseudo-random number betwen 0.0f and 1.0f.
 * @see MFRand(), MFRand_Range()
 */
float MFRand_Unit();

/**
 * Get a pseudo-random double precision number between 0.0f and 1.0f.
 * Get a pseudo-random double precision number between 0.0f and 1.0f.
 * @return Returns a double precision pseudo-random number betwen 0.0f and 1.0f.
 * @see MFRand(), MFRand_Unit()
 */
double MFRand_Double();

/**
 * Get a pseudo-random number within a specified range.
 * Get a pseudo-random number within a specified range.
 * @param min Minimum value.
 * @param max Maximum value.
 * @return Returns a pseudo-random number between \a min and \a max.
 * @see MFRand(), MFRand_Unit()
 */
float MFRand_Range(float min, float max);

#include "MFMath.inl"

#endif

/** @} */
