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

struct MFModel;

/**
 * @struct MFCollisionItem
 * Represents a Fuji collision item.
 */
struct MFCollisionItem;

/**
 * Collision item type.
 * Represents the type of a collision item.
 */
enum MFCollisionType
{
	MFCT_Unknown,	/**< Unknown collision item. */

	MFCT_Sphere,	/**< Item is a sphere primitive. */
	MFCT_Box,		/**< Item is a box primitive. */
	MFCT_Mesh,		/**< Item is a collision mesh (polygon soup). */
	MFCT_Field,		/**< Item is a collision field. */

	MFCT_Max,		/**< Maximum collision item. */
	MFCT_ForceInt = 0x7FFFFFFF	/**< Forces MFCollisionType to an int type. */
};

/**
 * Collision item flags.
 * Various flags that can be assigned to collision items.
 */
enum MFCollisionItemFlags
{
	MFCIF_Dynamic = 1,	/**< Collision item is dynamic, that is, an item that can move around the field. */
	MFCIF_Disabled = 2,	/**< Collision item is disabled. */

	MFCIF_ForceInt = 0x7FFFFFFF	/**< Forces MFCollisionItemFlags to an int type. */
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
	uint32 flags;			/**< Flags for each triangle. */
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

/*** Debug ***/

/**
 * Draw the collision debugging information.
 * This will draw the collision debugging information using the currently configured view.
 * @return None.
 */
MF_API void MFCollision_DebugDraw();

/*** Helper functions ***/

/**
 * Get nearest point on a line.
 * Find the nearest point on a line to an arbitrary point.
 * @param lineStart Start of line segment.
 * @param lineEnd End of line segmnt.
 * @param point Point of reference to find the closest point on the line.
 * @param pOutput Pointer to an MFVector that received the output.
 * @return Returns the nearest point on the ray to the specified point.
 */
MFVector& MFCollision_NearestPointOnLine(const MFVector& lineStart, const MFVector& lineEnd, const MFVector& point, MFVector *pOutput);

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
MF_API void MFCollision_MakeCollisionTriangleFromPoints(const MFVector &p0, const MFVector& p1, const MFVector& p2, MFCollisionTriangle *pTri);


/**** General collision tests ****/

/**
 * Test 2 axis aligned bounding boxes for intersection.
 * Tests 2 axis aligned bounding boxes for intersection.
 * @param min1 Min position of the first box.
 * @param max1 Max position of the first box.
 * @param min2 Min position of the second box.
 * @param max2 Max position of the second box.
 * @return Returns \a true if the boxes intersect.
 */
bool MFCollision_TestAABB(const MFVector &min1, const MFVector &max1, const MFVector &min2, const MFVector &max2);

/**
 * Test a ray for intersection with an arbitrary CollisionItem.
 * Tests a ray for intersection with an arbitrary CollisionItem.
 * @param rayPos Ray starting position.
 * @param rayDir Ray direction.
 * @param pItem Pointer to the item to test for intersection by the ray.
 * @param pResult Optional pointer to an MFRayIntersectionResult structire to receive details about the intersection.
 * @return Returns a pointer to the nearest CollisionItem intersected by the ray. Return value is NULL if no intersection occurred.
 */
MF_API MFCollisionItem* MFCollision_RayTest(const MFVector &rayPos, const MFVector &rayDir, MFCollisionItem *pItem, MFRayIntersectionResult *pResult);

/**
 * Test a sphere for intersection with an arbitrary CollisionItem.
 * Tests a sphere for intersection with an arbitrary CollisionItem.
 * @param spherePos World position of the sphere.
 * @param radius Radius of the sphere.
 * @param pItem Pointer to the item to test for intersection by the sphere.
 * @param pResult Optional pointer to an MFCollisionResult which will receive information about the intersection.
 * @return Returns a pointer to the nearest CollisionItem intersected by the sphere. Return value is NULL if no intersection occurred.
 */
MF_API MFCollisionItem* MFCollision_SphereTest(const MFVector &spherePos, float radius, MFCollisionItem *pItem, MFCollisionResult *pResult);

/**
 * Test a sweeping sphere for intersection with an arbitrary CollisionItem.
 * Tests a sweeping sphere for intersection with an arbitrary CollisionItem.
 * @param sweepSpherePos World position of the sweeping sphere.
 * @param sweepSphereVelocity The sweeping sphere's velocity vector.
 * @param sweepSphereRadius Radius of the sweeping sphere.
 * @param pItem Pointer to the item to test for intersection by the sweeping sphere.
 * @param pResult Optional pointer to an MFSweepSphereResult which will receive information about the intersection.
 * @return Returns a pointer to the nearest CollisionItem intersected by the sweeping sphere. Return value is NULL if no intersection occurred.
 */
MF_API MFCollisionItem* MFCollision_SweepSphereTest(const MFVector &sweepSpherePos, const MFVector &sweepSphereVelocity, float sweepSphereRadius, MFCollisionItem *pItem, MFSweepSphereResult *pResult);


/*** Arbitrary primitive intersections ***/

// ray intersections

/**
 * Intersect a ray with a plane.
 * Find the point where a ray intersects a plane.
 * @param rayPos Ray starting position.
 * @param rayDir Ray direction.
 * @param plane Vector representing the plane to intersect.
 * @param pResult Optional pointer to an MFRayIntersectionResult structure that receives details about the intersection.
 * @return Returns true if the ray intersects the plane.
 */
bool MFCollision_RayPlaneTest(const MFVector& rayPos, const MFVector& rayDir, const MFVector& plane, MFRayIntersectionResult *pResult = NULL);

/**
 * Intersect a ray with a slab.
 * Find the nearest point where a ray intersects a slab.
 * @param rayPos Ray starting position.
 * @param rayDir Ray direction.
 * @param plane Vector representing the plane to intersect.
 * @param slabHalfWidth Half the width of the slab, or rather, the distance from the plane to the surface.
 * @param pResult Optional pointer to an MFRayIntersectionResult structure that receives details about the intersection.
 * @return Returns true if the ray intersects the slab.
 */
MF_API bool MFCollision_RaySlabTest(const MFVector& rayPos, const MFVector& rayDir, const MFVector& plane, float slabHalfWidth, MFRayIntersectionResult *pResult = NULL);

/**
 * Intersect a ray with a sphere.
 * Find the nearest point where a ray intersects a sphere.
 * @param rayPos Ray starting position.
 * @param rayDir Ray direction.
 * @param spherePos Sphere position.
 * @param radius Sphere radius.
 * @param pResult Optional pointer to an MFRayIntersectionResult structure that receives details about the intersection.
 * @return Returns true if the ray intersects the sphere.
 */
MF_API bool MFCollision_RaySphereTest(const MFVector& rayPos, const MFVector& rayDir, const MFVector& spherePos, float radius, MFRayIntersectionResult *pResult = NULL);

/**
 * Intersect a ray with a cylinder.
 * Find the nearest point where a ray intersects a cylinder.
 * @param rayPos Ray starting position.
 * @param rayDir Ray direction.
 * @param cylinderPos Position of the base of the cylinder.
 * @param cylinderDir Vector along which the cylinder extends.
 * @param cylinderRadius Cylinder radius.
 * @param capped Specifies weather to test the cylinders caps or not. If false, it is treated as an infinite cylinder.
 * @param pResult Optional pointer to an MFRayIntersectionResult structure that receives details about the intersection.
 * @param pCylinderTime Optional pointer to a float that receives the time of intersection along the cylinders ray.
 * @return Returns true if the ray intersects the cylinder.
 */
MF_API bool MFCollision_RayCylinderTest(const MFVector& rayPos, const MFVector& rayDir, const MFVector& cylinderPos, const MFVector& cylinderDir, float cylinderRadius, bool capped, MFRayIntersectionResult *pResult = NULL, float *pCylinderTime = NULL);

/**
 * Intersect a ray with a capsule.
 * Find the nearest point where a ray intersects a capsule.
 * @param rayPos Ray starting position.
 * @param rayDir Ray direction.
 * @param capsulePos Position of the base of the capsule.
 * @param capsuleDir Vector along which the capsule extends.
 * @param capsuleRadius Capsule radius.
 * @param pResult Optional pointer to an MFRayIntersectionResult structure that receives details about the intersection.
 * @return Returns true if the ray intersects the capsule.
 */
MF_API bool MFCollision_RayCapsuleTest(const MFVector& rayPos, const MFVector& rayDir, const MFVector& capsulePos, const MFVector& capsuleDir, float capsuleRadius, MFRayIntersectionResult *pResult = NULL);

/**
 * Intersect a ray with a box.
 * Find the nearest point where a ray intersects a box.
 * @param rayPos Ray starting position.
 * @param rayDir Ray direction.
 * @param boxPos Position of the center of the box.
 * @param boxRadius Boxes radius along each axis.
 * @param pResult Optional pointer to an MFRayIntersectionResult structure that receives details about the intersection.
 * @return Returns true if the ray intersects the box.
 */
MF_API bool MFCollision_RayBoxTest(const MFVector& rayPos, const MFVector& rayDir, const MFVector& boxPos, const MFVector& boxRadius, MFRayIntersectionResult *pResult = NULL);


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
MF_API bool MFCollision_SpherePlaneTest(const MFVector& spherePos, float radius, const MFVector& plane, MFCollisionResult *pResult);

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
MF_API bool MFCollision_SphereSphereTest(const MFVector &pos1, float radius1, const MFVector &pos2, float radius2, MFCollisionResult *pResult);


// sweep sphere intersections

/**
 * Intersect a sweeping sphere with a sphere.
 * Find the details about an intersection between a sweeping sphere and a sphere.
 * @param sweepSpherePos Spheres initial position.
 * @param sweepSphereVelocity Spheres movement velocity.
 * @param sweepSphereRadius Radius of the sphere.
 * @param sphere Position of sphere to test against.
 * @param sphereRadius Radius of sphere to test against.
 * @param pResult Optional pointer to an MFSweepSphereResult structure to receive details about the collision.
 * @return Returns true if the spheres intersect.
 */
MF_API bool MFCollision_SweepSphereSphereTest(const MFVector &sweepSpherePos, const MFVector &sweepSphereVelocity, float sweepSphereRadius, const MFVector &sphere, float sphereRadius, MFSweepSphereResult *pResult);


/**
 * Intersect a sweeping sphere with a triangle.
 * Find the details about an intersection between a sweeping sphere and a triangle.
 * @param sweepSpherePos Spheres initial position.
 * @param sweepSphereVelocity Spheres movement velocity.
 * @param sweepSphereRadius Radius of the sphere.
 * @param tri Triangle to test for collision.
 * @param pResult Optional pointer to an MFSweepSphereResult structure to receive details about the collision.
 * @return Returns true if the sweeping sphere and triangle intersect.
 */
MF_API bool MFCollision_SweepSphereTriTest(const MFVector &sweepSpherePos, const MFVector &sweepSphereVelocity, float sweepSphereRadius, const MFCollisionTriangle &tri, MFSweepSphereResult *pResult);


/*** Collision item functions ***/

/**
 * Create a dynamic mesh collision item.
 * Creates a dynamic mesh collision item (polygon soup).
 * @param pItemName Name of the collision mesh.
 * @param numTris Number of tris in the mesh.
 * @return Returns a pointer to the newly created dynamic collision mesh.
 */
MF_API MFCollisionItem* MFCollision_CreateDynamicCollisionMesh(const char *pItemName, int numTris);

/**
 * Lock a dynamic collision mesh triangle buffer.
 * Locks and returns a pointer to a dynamic collision mesh triangle buffer.
 * @param pDynamicCollisionMesh The dynamic mesh collision item to lock.
 * @return Returns a pointer to an area in memory where the triangle buffer should be written.
 */
MF_API MFCollisionTriangle* MFCollision_LockDynamicCollisionMeshTriangleBuffer(MFCollisionItem *pDynamicCollisionMesh);

/**
 * Unlock a dynamic collision mesh triangle buffer.
 * Releases the lock on a dynamic collision mesh buffer.
 * @param pDynamicCollisionMesh The dynamic mesh collision item to unlock.
 * @return None.
 * @remarks The bounding volume for the mesh will be generated at this time.
 */
MF_API void MFCollision_UnlockDynamicCollisionMeshTriangleBuffer(MFCollisionItem *pDynamicCollisionMesh);

/**
 * Destroy a dynamic collision item.
 * Destroys a dynamic collision item.
 * @param pItem The MFCollisionItem to be destroyed.
 * @return None.
 */
MF_API void MFCollision_DestroyDynamicCollisionItem(MFCollisionItem *pItem);


/**** Collision field ****/

/**
 * Create a collision field.
 * Creates a collisoin field.
 * @param pFieldName Name of the collision field.
 * @param maximumItemCount Maximum number of items that can exist in the field.
 * @param cellSize The size of the cells in the x, y and z axiis.
 * @return Returns a pointer to the newly created collision field.
 */
MF_API MFCollisionItem* MFCollision_CreateField(const char *pFieldName, int maximumItemCount, const MFVector &cellSize);

/**
 * Add an item to a field.
 * Adds an MFCollisionItem to the specified collision field.
 * @param pField The field to add the item to.
 * @param pItem The item to be added to the field.
 * @param itemFlags 0, or a combination of values from the MFCollisionItemFlags enumerated type detailing the item being added.
 * @return None.
 */
MF_API void MFCollision_AddItemToField(MFCollisionItem *pField, MFCollisionItem *pItem, uint32 itemFlags);

/**
 * Add a model to a field.
 * Adds any collision items contained within an MFModel to the specified collision field.
 * @param pField The field to add the models collision items to.
 * @param pModel An MFModel that contains some collision items.
 * @return None.
 */
MF_API void MFCollision_AddModelToField(MFCollisionItem *pField, MFModel *pModel);

/**
 * Build a collision field.
 * Prepares a collision field for use after adding all the collision items.
 * @param pField The collision field to build.
 * @return None.
 * @remarks This must be called after all items have been added to the field. After this is called, no more items can be added to the field without first clearing the field.
 */
MF_API void MFCollision_BuildField(MFCollisionItem *pField);

/**
 * Clear a collision field.
 * Clear all items from a collision field.
 * @param pField The collision field to clear.
 * @return None.
 */
MF_API void MFCollision_ClearField(MFCollisionItem *pField);

/**
 * Destroy a collision field.
 * Destroys a collision field.
 * @param pField The collision field to destroy.
 * @return None.
 */
MF_API void MFCollision_DestroyField(MFCollisionItem *pField);


#include "MFCollision.inl"

#endif // _COLLISION_H

/** @} */
