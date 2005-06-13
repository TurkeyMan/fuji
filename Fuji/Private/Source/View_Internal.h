#if !defined(_VIEW_INTERNAL_H)
#define _VIEW_INTERNAL_H

#include "View.h"

// internal functions
void View_InitModule();
void View_DeinitModule();

// view structure
struct View
{
	static View defaultView;
	static MFRect defaultOrthoRect;

	MFRect orthoRect;

	Matrix projection;
	Matrix cameraMatrix;
	Matrix view;
	Matrix viewProj;

	float fov;
	float nearPlane, farPlane;
	float aspectRatio;

	bool viewDirty;
	bool viewProjDirty;
	bool isOrtho;
};

#endif