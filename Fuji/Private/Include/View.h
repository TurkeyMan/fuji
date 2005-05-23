#if !defined(_VIEW_H)
#define _VIEW_H

#include "Matrix.h"

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

void View_SetProjection(float fov = 0.0f, float nearPlane = 0.0f, float farPlane = 0.0f, float aspectRatio = 0.0f);
void View_SetOrtho(MFRect *pOrthoRect = NULL);
bool View_IsOrtho();

void View_SetCameraMatrix(const Matrix &cameraMatrix);

const Matrix& View_GetWorldToViewMatrix();
const Matrix& View_GetViewToScreenMatrix();
const Matrix& View_GetWorldToScreenMatrix();

Matrix* View_GetLocalToScreen(const Matrix& localToWorld, Matrix *pOutput);
Matrix* View_GetLocalToView(const Matrix& localToWorld, Matrix *pOutput);

#endif
