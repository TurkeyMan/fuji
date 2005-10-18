#if !defined(_COLLISION_H)
#define _COLLISION_H

#include "MFVector.h"
#include "MFMatrix.h"

struct MFCollisionResult
{
	MFVector intersectionPoint;
	MFVector normal;
	float depth;

	bool bCollide;
};

// helpers
MFVector MFCollision_NearestPointOnLine(const MFVector& lineStart, const MFVector& lineEnd, const MFVector& point);

// ray intersections
bool MFCollision_RaySphereTest(const MFVector& rayPos, const MFVector& rayDir, const MFVector& spherePos, float radius, float *pTime);
bool MFCollision_RayPlaneTest(const MFVector& rayPos, const MFVector& rayDir, const MFVector& plane, float *pTime);

// sphere intersections
bool MFCollision_SpherePlaneTest(const MFVector& spherePos, float radius, const MFVector& plane, MFCollisionResult *pResult);
bool MFCollision_SphereSphereTest(const MFVector &pos1, float radius1, const MFVector &pos2, float radius2, MFCollisionResult *pResult);

#include "MFCollision.inl"

#endif // _COLLISION_H
