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
	MFVector boundingSphere; /**< Bounding sphere (radius in the w) */
	MFVector min;  /**< Bounding box min coords */
	MFVector max;  /**< Bounding box max coords */
};

#endif

/** @} */
