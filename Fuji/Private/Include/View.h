#if !defined(_VIEW_H)
#define _VIEW_H

#include "MFMatrix.h"

// rect structure
struct MFRect
{
	float x, y;
	float width, height;
};

// interface functions
void View_Push();
void View_Pop();

void View_SetDefault();

void View_SetProjection(float fov = 0.0f, float aspectRatio = 0.0f, float nearPlane = 0.0f, float farPlane = 0.0f);
void View_GetProjection(float *pFov, float *pAspectRatio, float *pNearPlane, float *pFarPlane);

void View_SetOrtho(MFRect *pOrthoRect = NULL);
void View_GetOrtho(MFRect *pOrthoRect);

bool View_IsOrtho();

void View_SetCameraMatrix(const MFMatrix &cameraMatrix);
const MFMatrix& View_GetCameraMatrix();

const MFMatrix& View_GetWorldToViewMatrix();
const MFMatrix& View_GetViewToScreenMatrix();
const MFMatrix& View_GetWorldToScreenMatrix();

MFMatrix* View_GetLocalToScreen(const MFMatrix& localToWorld, MFMatrix *pOutput);
MFMatrix* View_GetLocalToView(const MFMatrix& localToWorld, MFMatrix *pOutput);

#endif
