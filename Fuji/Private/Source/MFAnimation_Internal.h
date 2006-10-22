#if !defined(_MFANIMATION_INTERNAL_H)
#define _MFANIMATION_INTERNAL_H

#include "MFAnimation.h"

struct MFAnimationTemplate;

void MFAnimation_InitModule();
void MFAnimation_DeinitModule();

struct MFAnimation
{
	MFAnimationTemplate *pTemplate;
	MFModel *pModel;

	int *pBoneMap;

	MFMatrix *pMatrices;
	MFMatrix *pCustomMatrices;

	float curFrame;
	float targetFrame;
	float tween;
};

struct MFAnimationFrame
{
	MFMatrix key;
//	MFQuaternion rot;
//	MFVector scale;
//	MFVector trans;
};

struct MFAnimationBone
{
	const char *pBoneName;

	float *pTime;
	MFAnimationFrame *pFrames;
	int numFrames;
};

struct MFAnimationTemplate
{
	union
	{
		uint32 IDtag;
		const char *pFilename;
	};

	const char *pName;

	MFAnimationBone *pBones;
	int numBones;

	float startTime, endTime;

	int refCount;
};

#endif
