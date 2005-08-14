#if !defined(_COLLISION_H)
#define _COLLISION_H

#include "MFVector.h"

bool Collision_SphereSphereTest(const MFVector &pos1, float radius1, const MFVector &pos2, float radius2);
MFVector Collision_NearestPointOnLine(const MFVector& lineStart, const MFVector& lineEnd, const MFVector& point);

#endif // _COLLISION_H
