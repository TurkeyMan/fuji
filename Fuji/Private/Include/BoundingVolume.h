#if !defined(_BONDING_VOLUME_H)
#define _BONDING_VOLUME_H

#include "Vector3.h"
#include "Vector4.h"

class BoundingVolume
{
public:
	Vector4 boundingSphere;
	Vector3 min, max;
};

class Frustum
{
public:
	Vector4 left, right, top, bottom, hither, yon;

	// return 0 (outside), 1 (inside), 2 (intersecting)
	int SphereWithin(const Vector4& sphere);
};

#endif
