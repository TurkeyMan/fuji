#if !defined(_BONDING_VOLUME_H)
#define _BONDING_VOLUME_H

#include "MFVector.h"

class BoundingVolume
{
public:
	MFVector boundingSphere;
	MFVector min, max;
};

class Frustum
{
public:
	MFVector left, right, top, bottom, hither, yon;

	// return 0 (outside), 1 (inside), 2 (intersecting)
	int SphereWithin(const MFVector& sphere);
};

#endif
