module fuji.collision;

public import fuji.fuji;
import fuji.matrix;
import fuji.model;

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
	Unknown,	/**< Unknown collision item. */

	Sphere,		/**< Item is a sphere primitive. */
	Box,		/**< Item is a box primitive. */
	Mesh,		/**< Item is a collision mesh (polygon soup). */
	Field		/**< Item is a collision field. */
}

/**
* Collision item flags.
* Various flags that can be assigned to collision items.
*/
enum MFCollisionItemFlags
{
	Dynamic = 1,	/**< Collision item is dynamic, that is, an item that can move around the field. */
	Disabled = 2,	/**< Collision item is disabled. */

	ForceInt = 0x7FFFFFFF	/**< Forces MFCollisionItemFlags to an int type. */
}

/**
* Collision triangle structure.
* Represents a triangle used for collision.
*/
struct MFCollisionTriangle
{
	MFVector plane;			/**< The triangle face plane. Plane normal represents the front face. The front face is defined by clockwise verts. */
	MFVector[3] edgePlanes;	/**< 3 Planes that bound each edge of the triangle. Plane normals point inwards. */
	MFVector[3] verts;		/**< 3 Triangle vertices. In clockwise order. */

	int[3] adjacent;		/**< Adjacent triangles. */
	uint flags;				/**< Flags for each triangle. */
}

/**
* Collision result structure.
* Represents details about a collision betwen 2 primitives.
*/
struct MFCollisionResult
{
	MFVector intersectionPoint;	/**< Center of intersection volume. */
	MFVector normal;			/**< Push out normal from second primitive. */
	float depth;				/**< Depth of intersection volume. */
	MFCollisionItem* pItem;		/**< Item that test subject collided against. */

	bool bCollide;				/**< If an intersection occurred. */
}

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
}

/**
* Sweep sphere result structure.
* Represents the results of a sweeping sphere with various primitives.
*/
struct MFSweepSphereResult
{
	MFVector surfaceNormal;			/**< Surface normal at the point of collision. */
	MFVector intersectionReaction;	/**< Reaction to be applied in the case if an intersection. */
	float time;						/**< Time of collision along spheres velocity vector. */
}


/*** Debug ***/

/**
* Draw the collision debugging information.
* This will draw the collision debugging information using the currently configured view.
* @return None.
*/
extern (C) void MFCollision_DebugDraw();

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
ref MFVector MFCollision_NearestPointOnLine(ref const(MFVector) lineStart, ref const(MFVector) lineEnd, ref const(MFVector) point, MFVector* pOutput)
{
	// calculate lines ray
	MFVector ray = lineEnd - lineStart;

	// see if a is the nearest point
	float dot_ta = ray.dot3(point - lineStart);
	if(dot_ta <= 0.0f)
	{
		*pOutput = lineStart;
		return *pOutput;
	}

	// see if b is the nearest point
	float dot_tb = (-ray).dot3(point - lineEnd);
	if(dot_tb <= 0.0f)
	{
		*pOutput = lineEnd;
		return *pOutput;
	}

	// return nearest point on line segment
	*pOutput = lineStart + (ray*  dot_ta*(1.0f / (dot_ta + dot_tb)));
	return* pOutput;
}

/**
* Generate a plane from 3 points.
* Generates a plane from 3 arbitrary points.
* @param p0 First point.
* @param p1 Second point.
* @param p2 Third point.
* @return Returns an MFVector containing the plane represented by the 3 given points.
*/
MFVector MFCollision_MakePlaneFromPoints(ref const(MFVector) p0, ref const(MFVector) p1, ref const(MFVector) p2)
{
	MFVector p = void;
	p = normalise!3(cross3(p1-p0, p2-p0));
	p.w = -dot3(p, p0);

	return p;
}

/**
* Generate a plane from a point and a normal.
* Generates a plane from a point and a normal.
* @param point Point on the plane.
* @param normal The plane's normal.
* @return Returns an MFVector containing the plane represented by the given point and normal.
*/
MFVector MFCollision_MakePlaneFromPointAndNormal(ref const(MFVector) point, ref const(MFVector) normal)
{
	MFVector p;

	p = normal;
	p.normalise!3();
	p.w = -dot3(point, p);

	return p;
}

/**
* Generate a collision triangle from 3 points.
* Generates a collision triangle from 3 points.
* @param p0 The first point.
* @param p1 The second point.
* @param p2 The third point.
* @param pTri Pointer to an MFCollisionTriangle structure to be generated from the points.
* @return None.
*/
extern (C) void MFCollision_MakeCollisionTriangleFromPoints(ref const(MFVector) p0, ref const(MFVector) p1, ref const(MFVector) p2, MFCollisionTriangle* pTri);


/**** General collision tests ****/

/**
* Test 2 axis aligned bounding boxes for intersection.
* Tests 2 axis aligned bounding boxes for intersection.
* @param min1 Min position of the first box.
* @param max1 Max position of the first box.
* @param min2 Min position of the second box.
* @param max2 Max position of the second box.
* @return Returns /a true if the boxes intersect.
*/
bool MFCollision_TestAABB(ref const(MFVector) min1, ref const(MFVector) max1, ref const(MFVector) min2, ref const(MFVector) max2)
{
	if(max1.x > min2.x && min1.x < max2.x &&
	   max1.y > min2.y && min1.y < max2.y &&
	   max1.z > min2.z && min1.z < max2.z)
	{
		return true;
	}

	return false;
}

/**
* Test a ray for intersection with an arbitrary CollisionItem.
* Tests a ray for intersection with an arbitrary CollisionItem.
* @param rayPos Ray starting position.
* @param rayDir Ray direction.
* @param pItem Pointer to the item to test for intersection by the ray.
* @param pResult Optional pointer to an MFRayIntersectionResult structire to receive details about the intersection.
* @return Returns a pointer to the nearest CollisionItem intersected by the ray. Return value is null if no intersection occurred.
*/
extern (C) MFCollisionItem* MFCollision_RayTest(ref const(MFVector) rayPos, ref const(MFVector) rayDir, MFCollisionItem* pItem, MFRayIntersectionResult* pResult);

/**
* Test a sphere for intersection with an arbitrary CollisionItem.
* Tests a sphere for intersection with an arbitrary CollisionItem.
* @param spherePos World position of the sphere.
* @param radius Radius of the sphere.
* @param pItem Pointer to the item to test for intersection by the sphere.
* @param pResult Optional pointer to an MFCollisionResult which will receive information about the intersection.
* @return Returns a pointer to the nearest CollisionItem intersected by the sphere. Return value is null if no intersection occurred.
*/
extern (C) MFCollisionItem* MFCollision_SphereTest(ref const(MFVector) spherePos, float radius, MFCollisionItem* pItem, MFCollisionResult* pResult);

/**
* Test a sweeping sphere for intersection with an arbitrary CollisionItem.
* Tests a sweeping sphere for intersection with an arbitrary CollisionItem.
* @param sweepSpherePos World position of the sweeping sphere.
* @param sweepSphereVelocity The sweeping sphere's velocity vector.
* @param sweepSphereRadius Radius of the sweeping sphere.
* @param pItem Pointer to the item to test for intersection by the sweeping sphere.
* @param pResult Optional pointer to an MFSweepSphereResult which will receive information about the intersection.
* @return Returns a pointer to the nearest CollisionItem intersected by the sweeping sphere. Return value is null if no intersection occurred.
*/
extern (C) MFCollisionItem* MFCollision_SweepSphereTest(ref const(MFVector) sweepSpherePos, ref const(MFVector) sweepSphereVelocity, float sweepSphereRadius, MFCollisionItem* pItem, MFSweepSphereResult* pResult);


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
bool MFCollision_RayPlaneTest(ref const(MFVector) rayPos, ref const(MFVector) rayDir, ref const(MFVector) plane, MFRayIntersectionResult* pResult = null)
{
	float a = dot3(plane, rayDir);

	// if ray is parallel to plane
	if(a > -0.0000001 && a < 0.0000001)
		return false;

	float t = -doth(rayPos, plane) / a;

	if(t < 0.0f || t > 1.0f)
		return false;

	if(pResult)
	{
		pResult.time = t;
		pResult.surfaceNormal = plane;
	}

	return true;
}

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
extern (C) bool MFCollision_RaySlabTest(ref const(MFVector) rayPos, ref const(MFVector) rayDir, ref const(MFVector) plane, float slabHalfWidth, MFRayIntersectionResult* pResult = null);

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
extern (C) bool MFCollision_RaySphereTest(ref const(MFVector) rayPos, ref const(MFVector) rayDir, ref const(MFVector) spherePos, float radius, MFRayIntersectionResult* pResult = null);

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
extern (C) bool MFCollision_RayCylinderTest(ref const(MFVector) rayPos, ref const(MFVector) rayDir, ref const(MFVector) cylinderPos, ref const(MFVector) cylinderDir, float cylinderRadius, bool capped, MFRayIntersectionResult* pResult = null, float* pCylinderTime = null);

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
extern (C) bool MFCollision_RayCapsuleTest(ref const(MFVector) rayPos, ref const(MFVector) rayDir, ref const(MFVector) capsulePos, ref const(MFVector) capsuleDir, float capsuleRadius, MFRayIntersectionResult* pResult = null);

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
extern (C) bool MFCollision_RayBoxTest(ref const(MFVector) rayPos, ref const(MFVector) rayDir, ref const(MFVector) boxPos, ref const(MFVector) boxRadius, MFRayIntersectionResult* pResult = null);


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
extern (C) bool MFCollision_SpherePlaneTest(ref const(MFVector) spherePos, float radius, ref const(MFVector) plane, MFCollisionResult* pResult);

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
extern (C) bool MFCollision_SphereSphereTest(ref const(MFVector) pos1, float radius1, ref const(MFVector) pos2, float radius2, MFCollisionResult* pResult);


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
extern (C) bool MFCollision_SweepSphereSphereTest(ref const(MFVector) sweepSpherePos, ref const(MFVector) sweepSphereVelocity, float sweepSphereRadius, ref const(MFVector) sphere, float sphereRadius, MFSweepSphereResult* pResult);


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
extern (C) bool MFCollision_SweepSphereTriTest(ref const(MFVector) sweepSpherePos, ref const(MFVector) sweepSphereVelocity, float sweepSphereRadius, ref const(MFCollisionTriangle) tri, MFSweepSphereResult* pResult);


/*** Collision item functions ***/

/**
* Create a dynamic mesh collision item.
* Creates a dynamic mesh collision item (polygon soup).
* @param pItemName Name of the collision mesh.
* @param numTris Number of tris in the mesh.
* @return Returns a pointer to the newly created dynamic collision mesh.
*/
extern (C) MFCollisionItem* MFCollision_CreateDynamicCollisionMesh(const char* pItemName, int numTris);

/**
* Lock a dynamic collision mesh triangle buffer.
* Locks and returns a pointer to a dynamic collision mesh triangle buffer.
* @param pDynamicCollisionMesh The dynamic mesh collision item to lock.
* @return Returns a pointer to an area in memory where the triangle buffer should be written.
*/
extern (C) MFCollisionTriangle* MFCollision_LockDynamicCollisionMeshTriangleBuffer(MFCollisionItem* pDynamicCollisionMesh);

/**
* Unlock a dynamic collision mesh triangle buffer.
* Releases the lock on a dynamic collision mesh buffer.
* @param pDynamicCollisionMesh The dynamic mesh collision item to unlock.
* @return None.
* @remarks The bounding volume for the mesh will be generated at this time.
*/
extern (C) void MFCollision_UnlockDynamicCollisionMeshTriangleBuffer(MFCollisionItem* pDynamicCollisionMesh);

/**
* Destroy a dynamic collision item.
* Destroys a dynamic collision item.
* @param pItem The MFCollisionItem to be destroyed.
* @return None.
*/
extern (C) void MFCollision_DestroyDynamicCollisionItem(MFCollisionItem* pItem);


/**** Collision field ****/

/**
* Create a collision field.
* Creates a collisoin field.
* @param pFieldName Name of the collision field.
* @param maximumItemCount Maximum number of items that can exist in the field.
* @param cellSize The size of the cells in the x, y and z axiis.
* @return Returns a pointer to the newly created collision field.
*/
extern (C) MFCollisionItem* MFCollision_CreateField(const char* pFieldName, int maximumItemCount, ref const(MFVector) cellSize);

/**
* Add an item to a field.
* Adds an MFCollisionItem to the specified collision field.
* @param pField The field to add the item to.
* @param pItem The item to be added to the field.
* @param itemFlags 0, or a combination of values from the MFCollisionItemFlags enumerated type detailing the item being added.
* @return None.
*/
extern (C) void MFCollision_AddItemToField(MFCollisionItem* pField, MFCollisionItem* pItem, uint itemFlags);

/**
* Add a model to a field.
* Adds any collision items contained within an MFModel to the specified collision field.
* @param pField The field to add the models collision items to.
* @param pModel An MFModel that contains some collision items.
* @return None.
*/
extern (C) void MFCollision_AddModelToField(MFCollisionItem* pField, MFModel* pModel);

/**
* Build a collision field.
* Prepares a collision field for use after adding all the collision items.
* @param pField The collision field to build.
* @return None.
* @remarks This must be called after all items have been added to the field. After this is called, no more items can be added to the field without first clearing the field.
*/
extern (C) void MFCollision_BuildField(MFCollisionItem* pField);

/**
* Clear a collision field.
* Clear all items from a collision field.
* @param pField The collision field to clear.
* @return None.
*/
extern (C) void MFCollision_ClearField(MFCollisionItem* pField);

/**
* Destroy a collision field.
* Destroys a collision field.
* @param pField The collision field to destroy.
* @return None.
*/
extern (C) void MFCollision_DestroyField(MFCollisionItem* pField);

