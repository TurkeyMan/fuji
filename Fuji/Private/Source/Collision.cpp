#include "Common.h"
#include "Collision.h"


bool Collision_SphereSphereTest(const Vector3 &pos1, float radius1, const Vector3 &pos2, float radius2)
{
	//find difference between the two objects = Distance and return bool answer
	Vector3 collisionDistance = pos2 - pos1;
	return collisionDistance.MagSquared() < radius1*radius1 + radius2*radius2;
}
