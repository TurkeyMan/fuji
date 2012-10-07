#if !defined(_MFVIEW_INTERNAL_H)
#define _MFVIEW_INTERNAL_H

#include "MFView.h"

// internal functions
MFInitStatus MFView_InitModule();
void MFView_DeinitModule();

// view structure
struct MFView
{
	static MFView defaultView;
	static MFRect defaultOrthoRect;

	MFRect orthoRect;

	MFMatrix projection;
	MFMatrix cameraMatrix;
	MFMatrix view;
	MFMatrix viewProj;

	float fov;
	float nearPlane, farPlane;
	float aspectRatio;

	bool viewDirty;
	bool viewProjDirty;
	bool projDirty;
	bool isOrtho;
	bool customProjection;
};

#endif
