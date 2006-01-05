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


struct MFCollisionField;
struct MFCollisionItem;

enum MFCollisionType
{
	MFCT_Unknown,

	MFCT_Sphere,
	MFCT_Box,
	MFCT_Mesh,
	MFCT_Field,

	MFCT_Max,
	MFCT_ForceInt = 0x7FFFFFFF
};

enum MFCollisionItemFlags
{
	MFCIF_Dynamic = 1,
	MFCIF_Disabled = 2,

	MFCIF_ForceInt = 0x7FFFFFFF
};

/**
 * Collision result structure.
 * Represents details about a collision betwen 2 primitives.
 */
struct MFCollisionResult
{
	MFVector intersectionPoint;	/**< Center of intersection volume. */
	MFVector normal;			/**< Push out normal from second primitive. */
	float depth;				/**< Depth of intersection volume. */
	MFCollisionItem *pItem;		/**< Item that test subject collided against. */

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

/**
 * Test a ray for intersection with an arbitrary CollisionItem.
 * Tests a ray for intersection with an arbitrary CollisionItem.
 * @param rayPos Ray starting position.
 * @param rayDir Ray direction
 * @param pRootItem Pointer to the item to test for intersection by the ray.
 * @param pTime Optional pointer to the time of intersection along the ray.
 * @return Returns a pointer to the nearest CollisionItem intersected by the ray. Return value is NULL if no intersection occurred.
 */
MFCollisionItem* MFCollision_RayTest(const MFVector& rayPos, const MFVector& rayDir, MFCollisionItem *pRootItem, float *pTime);

/**
 * Test a ray for intersection with an arbitrary CollisionItem.
 * Tests a ray for intersection with an arbitrary CollisionItem.
 * @param spherePos World position of the sphere.
 * @param radius Radius of the sphere
 * @param pRootItem Pointer to the item to test for intersection by the sphere.
 * @param pResult Optional pointer to an MFCollisionResult which will receive information about the intersection.
 * @return Returns a pointer to the nearest CollisionItem intersected by the ray. Return value is NULL if no intersection occurred.
 */
MFCollisionItem* MFCollision_SphereTest(const MFVector& spherePos, float radius, MFCollisionItem *pRootItem, MFCollisionResult *pResult);


MFCollisionItem* MFCollision_CreateField(int maximumItemCount, const MFVector &cellSize);

void MFCollision_AddItemToField(MFCollisionItem *pField, MFCollisionItem *pItem, uint32 itemFlags);

void MFCollision_BuildField(MFCollisionItem *pField);

void MFCollision_ClearField(MFCollisionItem *pField);

void MFCollision_DestroyField(MFCollisionItem *pField);

/*** Arbitrary primitive intersections ***/

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
