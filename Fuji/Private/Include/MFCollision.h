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
#include "MFBoundingVolume.h"


struct MFCollisionItem;
struct MFModel;

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
 * Collision triangle structure.
 * Represents a triangle used for collision.
 */
struct MFCollisionTriangle
{
	MFVector plane;			/**< The triangle face plane. Plane normal represents the front face. The front face is defined by clockwise verts. */
	MFVector edgePlanes[3];	/**< 3 Planes that bound each edge of the triangle. Plane normals point inwards. */
	MFVector verts[3];		/**< 3 Triangle vertices. In clockwise order. */

	int adjacent[3];		/**< Adjacent triangles. */
	uint32 padding;			/**< Pad structure to 16 bytes. */
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
 * Ray intersection result structure.
 * Represents the results of a ray intersection with various primitives.
 */
struct MFRayIntersectionResult
{
  MFVector surfaceNormal;		/**< Surface normal at the point of intersection. */
  float time;					/**< Time of intersection along ray. */

//  MFVector intersectPoint;
//  MFVector surfaceColour;
};

/**
 * Sweep sphere result structure.
 * Represents the results of a sweeping sphere with various primitives.
 */
struct MFSweepSphereResult
{
  MFVector surfaceNormal;			/**< Surface normal at the point of collision. */
  MFVector intersectionReaction;	/**< Reaction to be applied in the case if an intersection. */
  float time;						/**< Time of collision along spheres velocity vector. */
};

/*** Helper functions ***/

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
 * Generate a plane from 3 points.
 * Generates a plane from 3 arbitrary points.
 * @param p0 First point.
 * @param p1 Second point.
 * @param p2 Third point.
 * @return Returns an MFVector containing the plane represented by the 3 given points.
 */
MFVector MFCollision_MakePlaneFromPoints(const MFVector &p0, const MFVector &p1, const MFVector &p2);

/**
 * Generate a plane from a point and a normal.
 * Generates a plane from a point and a normal.
 * @param point Point on the plane.
 * @param normal The plane's normal.
 * @return Returns an MFVector containing the plane represented by the given point and normal.
 */
MFVector MFCollision_MakePlaneFromPointAndNormal(const MFVector &point, const MFVector &normal);

/**
 * Generate a collision triangle from 3 points.
 * Generates a collision triangle from 3 points.
 * @param p0 The first point.
 * @param p1 The second point.
 * @param p2 The third point.
 * @param pTri Pointer to an MFCollisionTriangle structure to be generated from the points.
 * @return None.
 */
void MFCollision_MakeCollisionTriangleFromPoints(const MFVector &p0, const MFVector& p1, const MFVector& p2, MFCollisionTriangle *pTri);


/**** General collision tests ****/

bool MFCollision_TestAABB(const MFVector &min1, const MFVector &max1, const MFVector &min2, const MFVector &max2);

/**
 * Test a ray for intersection with an arbitrary CollisionItem.
 * Tests a ray for intersection with an arbitrary CollisionItem.
 * @param rayPos Ray starting position.
 * @param rayDir Ray direction
 * @param pItem Pointer to the item to test for intersection by the ray.
 * @param pResult Optional pointer to an MFCollisionResult structire to receive details about the intersection.
 * @return Returns a pointer to the nearest CollisionItem intersected by the ray. Return value is NULL if no intersection occurred.
 */
MFCollisionItem* MFCollision_RayTest(const MFVector &rayPos, const MFVector &rayDir, MFCollisionItem *pItem, MFRayIntersectionResult *pResult);

/**
 * Test a ray for intersection with an arbitrary CollisionItem.
 * Tests a ray for intersection with an arbitrary CollisionItem.
 * @param spherePos World position of the sphere.
 * @param radius Radius of the sphere
 * @param pItem Pointer to the item to test for intersection by the sphere.
 * @param pResult Optional pointer to an MFCollisionResult which will receive information about the intersection.
 * @return Returns a pointer to the nearest CollisionItem intersected by the sphere. Return value is NULL if no intersection occurred.
 */
MFCollisionItem* MFCollision_SphereTest(const MFVector &spherePos, float radius, MFCollisionItem *pItem, MFCollisionResult *pResult);

MFCollisionItem* MFCollision_SweepSphereTest(const MFVector &sweepSpherePos, const MFVector &sweepSphereVelocity, float sweepSphereRadius, MFCollisionItem *pItem, MFSweepSphereResult *pResult);


/*** Arbitrary primitive intersections ***/

// ray intersections

/**
 * Intersect a ray with a plane.
 * Find the point where a ray intersects a plane.
 * @param rayPos Ray starting position.
 * @param rayDir Ray direction
 * @param plane Vector representing the plane to intersect.
 * @param pResult Optional pointer to an MFRayIntersectionResult structure that receives details about the intersection.
 * @return Returns true if the ray intersects the plane.
 */
bool MFCollision_RayPlaneTest(const MFVector& rayPos, const MFVector& rayDir, const MFVector& plane, MFRayIntersectionResult *pResult = NULL);

/**
 * Intersect a ray with a slab.
 * Find the nearest point where a ray intersects a slab.
 * @param rayPos Ray starting position.
 * @param rayDir Ray direction
 * @param plane Vector representing the plane to intersect.
 * @param slabHalfWidth Half the width of the slab, or rather, the distance from the plane to the surface.
 * @param pResult Optional pointer to an MFRayIntersectionResult structure that receives details about the intersection.
 * @return Returns true if the ray intersects the slab.
 */
bool MFCollision_RaySlabTest(const MFVector& rayPos, const MFVector& rayDir, const MFVector& plane, float slabHalfWidth, MFRayIntersectionResult *pResult = NULL);

/**
 * Intersect a ray with a sphere.
 * Find the nearest point where a ray intersects a sphere.
 * @param rayPos Ray starting position.
 * @param rayDir Ray direction
 * @param spherePos Sphere position.
 * @param radius Sphere radius.
 * @param pResult Optional pointer to an MFRayIntersectionResult structure that receives details about the intersection.
 * @return Returns true if the ray intersects the sphere.
 */
bool MFCollision_RaySphereTest(const MFVector& rayPos, const MFVector& rayDir, const MFVector& spherePos, float radius, MFRayIntersectionResult *pResult = NULL);

/**
 * Intersect a ray with a cylinder.
 * Find the nearest point where a ray intersects a cylinder.
 * @param rayPos Ray starting position.
 * @param rayDir Ray direction
 * @param cylinderPos Position of the base of the cylinder.
 * @param cylinderDir Vector along which the cylinder extends.
 * @param cylinderRadius Cylinder radius.
 * @param capped Specifies weather to test the cylinders caps or not. If false, it is treated as an infinite cylinder.
 * @param pResult Optional pointer to an MFRayIntersectionResult structure that receives details about the intersection.
 * @param pCylinderTime Optional pointer to a float that receives the time of intersection along the cylinders ray.
 * @return Returns true if the ray intersects the cylinder.
 */
bool MFCollision_RayCylinderTest(const MFVector& rayPos, const MFVector& rayDir, const MFVector& cylinderPos, const MFVector& cylinderDir, float cylinderRadius, bool capped, MFRayIntersectionResult *pResult = NULL, float *pCylinderTime = NULL);

/**
 * Intersect a ray with a capsule.
 * Find the nearest point where a ray intersects a capsule.
 * @param rayPos Ray starting position.
 * @param rayDir Ray direction
 * @param capsulePos Position of the base of the capsule.
 * @param capsuleDir Vector along which the capsule extends.
 * @param capsuleRadius Capsule radius.
 * @param pResult Optional pointer to an MFRayIntersectionResult structure that receives details about the intersection.
 * @return Returns true if the ray intersects the capsule.
 */
bool MFCollision_RayCapsuleTest(const MFVector& rayPos, const MFVector& rayDir, const MFVector& capsulePos, const MFVector& capsuleDir, float capsuleRadius, MFRayIntersectionResult *pResult = NULL);

/**
 * Intersect a ray with a box.
 * Find the nearest point where a ray intersects a box.
 * @param rayPos Ray starting position.
 * @param rayDir Ray direction
 * @param boxPos Position of the center of the box.
 * @param boxRadius Boxes radius along each axis.
 * @param pResult Optional pointer to an MFRayIntersectionResult structure that receives details about the intersection.
 * @return Returns true if the ray intersects the box.
 */
bool MFCollision_RayBoxTest(const MFVector& rayPos, const MFVector& rayDir, const MFVector& boxPos, const MFVector& boxRadius, MFRayIntersectionResult *pResult = NULL);


// sphere intersections

/**
 * Intersect a sphere with a plane.
 * Find the details about an intersection between a sphere and a plane.
 * @param spherePos Position of sphere.
 * @param radius Radius of sphere.
 * @param plane Vector representing the plane for intersection.
 * @param pResult Optional pointer to an MFCollisionResult structire to receive details about the intersection.
 * @return Returns true if the sphere and plane intersect.
 */
bool MFCollision_SpherePlaneTest(const MFVector& spherePos, float radius, const MFVector& plane, MFCollisionResult *pResult);

/**
 * Intersect a sphere with a sphere.
 * Find the details about an intersection between a sphere and another sphere.
 * @param pos1 Position of first sphere.
 * @param radius1 Radius of first sphere.
 * @param pos2 Position of second sphere.
 * @param radius2 Radius of second sphere.
 * @param pResult Optional pointer to an MFCollisionResult structire to receive details about the intersection.
 * @return Returns true if the spheres intersect.
 */
bool MFCollision_SphereSphereTest(const MFVector &pos1, float radius1, const MFVector &pos2, float radius2, MFCollisionResult *pResult);


// sweep sphere intersections

/**
 * Intersect a sweeping sphere with a triangle.
 * Find the details about an intersection between a sweeping sphere and a triangle.
 * @param sweepSpherePos Spheres initial position.
 * @param sweepSphereVelocity Spheres movement velocity.
 * @param sweepSphereRadius Radius of the sphere.
 * @param sphere Position of sphere to test against.
 * @param sphereRadius Radius of sphere to test against.
 * @param pResult Optional pointer to an MFSweepSphereResult structure to receive details about the collision.
 * @return Returns true if the spheres intersect.
 */
bool MFCollision_SweepSphereSphereTest(const MFVector &sweepSpherePos, const MFVector &sweepSphereVelocity, float sweepSphereRadius, const MFVector &sphere, float sphereRadius, MFSweepSphereResult *pResult);


/**
 * Intersect a sweeping sphere with a triangle.
 * Find the details about an intersection between a sweeping sphere and a triangle.
 * @param sweepSpherePos Spheres initial position.
 * @param sweepSphereVelocity Spheres movement velocity.
 * @param sweepSphereRadius Radius of the sphere.
 * @param tri Triangle to test for collision.
 * @param pResult Optional pointer to an MFSweepSphereResult structure to receive details about the collision.
 * @return Returns true if the spheres intersect.
 */
bool MFCollision_SweepSphereTriTest(const MFVector &sweepSpherePos, const MFVector &sweepSphereVelocity, float sweepSphereRadius, const MFCollisionTriangle &tri, MFSweepSphereResult *pResult);


/*** Collision item functions ***/

MFCollisionItem* MFCollision_CreateDynamicCollisionMesh(const char *pItemName, int numTris);

MFCollisionTriangle* MFCollision_GetDynamicCollisionMeshTriangleBuffer(MFCollisionItem *pDynamicCollisionMesh);

void MFCollision_CalculateDynamicBoundingVolume(MFCollisionItem *pItem);

void MFCollision_DestroyDynamicCollisionItem(MFCollisionItem *pItem);


/**** Collision field ****/

MFCollisionItem* MFCollision_CreateField(const char *pFieldName, int maximumItemCount, const MFVector &cellSize);

void MFCollision_AddItemToField(MFCollisionItem *pField, MFCollisionItem *pItem, uint32 itemFlags);

void MFCollision_AddModelToField(MFCollisionItem *pField, MFModel *pModel);

void MFCollision_BuildField(MFCollisionItem *pField);

void MFCollision_ClearField(MFCollisionItem *pField);

void MFCollision_DestroyField(MFCollisionItem *pField);


#include "MFCollision.inl"

#endif // _COLLISION_H

/** @} */
