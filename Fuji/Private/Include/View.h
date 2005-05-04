#if !defined(_VIEW_H)
#define _VIEW_H

#include "Matrix.h"

struct MFRect
{
	float x, y;
	float width, height;
};

void View_InitModule();
void View_DeinitModule();

void View_Push();
void View_Pop();

void View_SetDefault();

void View_SetProjection(float fov = 0.0f, float aspectRatio = 0.0f);
void View_SetOrtho(MFRect *pOrthoRect = NULL);
bool View_IsOrtho();

void View_SetCameraMatrix(const Matrix &viewMat);

const Matrix& View_GetWorldToViewMatrix();
const Matrix& View_GetViewToScreenMatrix();
const Matrix& View_GetWorldToScreenMatrix();

Matrix* View_GetLocalToScreen(const Matrix& localToWorld, Matrix *pOutput);
Matrix* View_GetLocalToView(const Matrix& localToWorld, Matrix *pOutput);

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
