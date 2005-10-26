#if !defined(_MFVIEW_H)
#define _MFVIEW_H

#include "MFMatrix.h"

// interface functions
void MFView_Push();
void MFView_Pop();

void MFView_SetDefault();

void MFView_ConfigureProjection(float fieldOfView, float nearPlane, float farPlane);
void MFView_SetAspectRatio(float aspectRatio);

void MFView_SetProjection();
void MFView_SetOrtho(MFRect *pOrthoRect = NULL);

void MFView_GetOrthoRect(MFRect *pOrthoRect);

bool MFView_IsOrtho();

void MFView_SetCameraMatrix(const MFMatrix &cameraMatrix);
const MFMatrix& MFView_GetCameraMatrix();

const MFMatrix& MFView_GetWorldToViewMatrix();
const MFMatrix& MFView_GetViewToScreenMatrix();
const MFMatrix& MFView_GetWorldToScreenMatrix();

MFMatrix* MFView_GetLocalToScreen(const MFMatrix& localToWorld, MFMatrix *pOutput);
MFMatrix* MFView_GetLocalToView(const MFMatrix& localToWorld, MFMatrix *pOutput);

#endif
