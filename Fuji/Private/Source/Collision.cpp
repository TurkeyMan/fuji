#include "Common.h"
#include "Collision.h"


bool Collision_SphereSphereTest(Vector3 pos1, float radius1, Vector3 pos2, float radius2)
{
	//find difference between the two objects = Distance and return bool answer
	Vector3 CollisionDistance = pos2 - pos1;
	return CollisionDistance.Magnitude() < radius1+radius2;
}
