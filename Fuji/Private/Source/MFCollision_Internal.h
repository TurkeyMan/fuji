#if !defined(_MFCOLLISION_INTERNAL_H)
#define _MFCOLLISION_INTERNAL_H

#include "MFCollision.h"
#include "MFBoundingVolume.h"
#include "MFPtrList.h"

struct MFCollisionTemplate
{
	MFBoundingVolume boundingVolume;
	uint16 type;
};

struct MFCollisionItem
{
	MFMatrix worldPos;
	MFCollisionTemplate *pTemplate;
	uint16 refCount;
	uint16 flags;
};

struct MFCollisionSphere : public MFCollisionTemplate
{
	float radius;	// this could be jammed into the w of the position
};

struct MFCollisionBox : public MFCollisionTemplate
{
	MFVector radius;	// this could easilly be jammed into the 4th column of the matrix
};

struct MFCollisionField : public MFCollisionTemplate
{
	MFVector fieldMin;
	MFVector fieldMax;
	MFVector cellSize;

	int width, height, depth;

	MFPtrList<MFCollisionItem> itemList;

	MFCollisionItem ***pppItems;
};

MFCollisionItem* MFCollision_RayFieldTest(const MFVector& rayPos, const MFVector& rayDir, MFCollisionItem *pField, MFRayIntersectionResult *pResult);

#endif // _MFCOLLISION_INTERNAL_H
