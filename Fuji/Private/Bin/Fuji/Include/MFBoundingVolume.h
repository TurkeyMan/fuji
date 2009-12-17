/**
 * @file MFBoundingVolume.h
 * @brief Bounding Volume structure.
 * @author Manu Evans
 * @defgroup MFBoundingVolume Bounding Volume
 * @{
 */

#if !defined(_MFBOUNDING_VOLUME_H)
#define _MFBOUNDING_VOLUME_H

#include "MFVector.h"

/**
 * Bounding volume.
 * Represents a bounding volume.
 */
struct MFBoundingVolume
{
public:
	MFVector boundingSphere;	/**< Bounding sphere (radius in the w) */
	MFVector min;				/**< Bounding box min coords */
	MFVector max;				/**< Bounding box max coords */
};

/**
 * Test 2 MFBoundingVolumes for intersection.
 * Tests 2 MFBoundingVolumes for intersection.
 * @param v1 The first bounding volume.
 * @param v2 The second bounding volume.
 * @return Returns /a true if the bounding volumes intersect.
 */
bool MFBoundingVolume_Test(const MFBoundingVolume &v1, const MFBoundingVolume &v2);


// internal (seems silly to make an MFBoundingVolume.inl file for this one function)

inline bool MFBoundingVolume_Test(const MFBoundingVolume &v1, const MFBoundingVolume &v2)
{
	MFVector diff = v2.boundingSphere - v1.boundingSphere;
	if(diff.MagSquared3() >= v1.boundingSphere.w*v1.boundingSphere.w + v2.boundingSphere.w*v2.boundingSphere.w)
		return false;

	if(v1.max.x > v2.min.x && v1.min.x < v2.max.x &&
		v1.max.y > v2.min.y && v1.min.y < v2.max.y &&
		  v1.max.z > v2.min.z && v1.min.z < v2.max.z)
	{
		return true;
	}

	return false;
}

#endif

/** @} */
