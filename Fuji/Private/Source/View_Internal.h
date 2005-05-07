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

	Matrix projection;
	Matrix view;

	Matrix viewProj;
	bool viewProjDirty;

	float fov;
	float aspectRatio;

	MFRect orthoRect;
	bool isOrtho;
};

#endif