/**
 * @file MFCollision.h
 * @brief A set of functions to perform collision operations between various primitives.
 * @author Manu Evans
 * @defgroup MFCollision Collision Functions
 * @{
 */

#if !defined(_COLLISION_H)
#define _COLLISION_H

#include "MFVector.h"
#include "MFMatrix.h"

/**
 * Collision result structure.
 * Represents details about a collision betwen 2 primitives.
 */
struct MFCollisionResult
{
	MFVector intersectionPoint;	/**< Center of intersection volume. */
	MFVector normal;			/**< Push out normal from second primitive. */
	float depth;				/**< Depth of intersection volume. */

	bool bCollide;				/**< If an intersection occurred. */
};

/**
 * Get nearest point on a line.
 * Find the nearest point on a line to an arbitrary point.
 * @param lineStart Start of line segment.
 * @param lineEnd End of line segmnt.
 * @param point Point of reference to find the closest point on the line.
 * @return Returns the nearest point on the ray to the specified point.
 */
MFVector MFCollision_NearestPointOnLine(const MFVector& lineStart, const MFVector& lineEnd, const MFVector& point);

// ray intersections

/**
 * Intersect a ray with a sphere.
 * Find the point where a ray intersects a sphere.
 * @param rayPos Ray starting position.
 * @param rayDir Ray direction
 * @param spherePos Sphere position.
 * @param radius Sphere radius.
 * @param pTime Pointer to a float to receive the time at which the ray intersects the sphere.
 * @return Returns true if the ray intersects the sphere.
 * @see MFCollision_RayPlaneTest()
 */
bool MFCollision_RaySphereTest(const MFVector& rayPos, const MFVector& rayDir, const MFVector& spherePos, float radius, float *pTime);

/**
 * Intersect a ray with a plane.
 * Find the point where a ray intersects a plane.
 * @param rayPos Ray starting position.
 * @param rayDir Ray direction
 * @param plane Vector representing the plane to intersect.
 * @param pTime Pointer to a float to receive the time at which the ray intersects the plane.
 * @return Returns true if the ray intersects the plane.
 * @see MFCollision_RaySphereTest()
 */
bool MFCollision_RayPlaneTest(const MFVector& rayPos, const MFVector& rayDir, const MFVector& plane, float *pTime);

// sphere intersections

/**
 * Intersect a sphere with a plane.
 * Find the details about an intersection between a sphere and a plane.
 * @param spherePos Position of sphere.
 * @param radius Radius of sphere.
 * @param plane Vector representing the plane for intersection.
 * @param pResult Pointer to an MFCollisionResult structire to receive details about the intersection.
 * @return Returns true if the sphere and plane intersect.
 * @see MFCollision_SphereSphereTest()
 */
bool MFCollision_SpherePlaneTest(const MFVector& spherePos, float radius, const MFVector& plane, MFCollisionResult *pResult);

/**
 * Intersect a sphere with a sphere.
 * Find the details about an intersection between a sphere and another sphere.
 * @param pos1 Position of first sphere.
 * @param radius1 Radius of first sphere.
 * @param pos2 Position of second sphere.
 * @param radius2 Radius of second sphere.
 * @param pResult Pointer to an MFCollisionResult structire to receive details about the intersection.
 * @return Returns true if the spheres intersect.
 * @see MFCollision_SpherePlaneTest()
 */
bool MFCollision_SphereSphereTest(const MFVector &pos1, float radius1, const MFVector &pos2, float radius2, MFCollisionResult *pResult);

#include "MFCollision.inl"

#endif // _COLLISION_H

/** @} */
