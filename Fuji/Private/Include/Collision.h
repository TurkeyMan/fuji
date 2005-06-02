#if !defined(_COLLISION_H)
#define _COLLISION_H

#include "Vector3.h"
#include "Vector4.h"

bool Collision_SphereSphereTest(const Vector3 &pos1, float radius1, const Vector3 &pos2, float radius2);
Vector3 Collision_NearestPointOnLine(const Vector3& lineStart, const Vector3& lineEnd, const Vector3& point);

#endif // _COLLISION_H
