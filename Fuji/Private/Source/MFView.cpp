#include "Fuji.h"
#include "MFHeap.h"
#include "Display_Internal.h"
#include "MFView_Internal.h"
#include "MFSystem.h"

#include <math.h>

MFView MFView::defaultView;
MFRect MFView::defaultOrthoRect;

MFView *gpViewStack = NULL;
MFView *pCurrentView = NULL;

void MFView_InitModule()
{
	// allocate view stack
	gpViewStack = (MFView*)MFHeap_Alloc(sizeof(MFView) * gDefaults.view.maxViewsOnStack);

	// set default ortho rect
	MFView::defaultOrthoRect.x = gDefaults.view.orthoMinX;
	MFView::defaultOrthoRect.y = gDefaults.view.orthoMinY;
	MFView::defaultOrthoRect.width = gDefaults.view.orthoMaxX;
	MFView::defaultOrthoRect.height = gDefaults.view.orthoMaxY;

	// initialise default view
	memset(&MFView::defaultView, 0, sizeof(MFView));
	pCurrentView = &MFView::defaultView;

	pCurrentView->cameraMatrix = MFMatrix::identity;
	pCurrentView->view = MFMatrix::identity;
	pCurrentView->viewProj = MFMatrix::identity;

	MFView_SetOrtho(&MFView::defaultOrthoRect);
	MFView_ConfigureProjection(MFDEGREES(gDefaults.view.defaultFOV), gDefaults.view.defaultNearPlane, gDefaults.view.defaultFarPlane);
	MFView_SetAspectRatio(gDefaults.view.defaultAspect);
	MFView_SetProjection();

	pCurrentView = gpViewStack;
	MFView_SetDefault();
}

void MFView_DeinitModule()
{
	/// free view stack
	if(gpViewStack)
	{
		MFHeap_Free(gpViewStack);
		gpViewStack = NULL;
	}
}

void MFView_Push()
{
	DBGASSERT(pCurrentView - gpViewStack < (int)gDefaults.view.maxViewsOnStack, "Error: Exceeded maximum views on the stack. Increase 'gDefaults.view.maxViewsOnStack'.");

	// push view pointer foreward
	pCurrentView++;
	*pCurrentView = pCurrentView[-1];
}

void MFView_Pop()
{
	DBGASSERT(pCurrentView > gpViewStack, "Error: No views on the stack!");

	pCurrentView--;
}


void MFView_SetDefault()
{
	*pCurrentView = MFView::defaultView;
}

void MFView_ConfigureProjection(float fieldOfView, float nearPlane, float farPlane)
{
	pCurrentView->fov = fieldOfView;
	pCurrentView->nearPlane = nearPlane;
	pCurrentView->farPlane = farPlane;
}

void MFView_SetAspectRatio(float aspectRatio)
{
	pCurrentView->aspectRatio = aspectRatio;
}

void MFView_SetProjection()
{
	CALLSTACK;

	pCurrentView->isOrtho = false;
	pCurrentView->viewProjDirty = true;

	// construct and apply perspective projection
	float zn = pCurrentView->nearPlane;
	float zf = pCurrentView->farPlane;

	float a = pCurrentView->fov * 0.5f;

	float h = MFCos(a) / MFSin(a);
	float w = h / pCurrentView->aspectRatio;

	pCurrentView->projection.m[0][0] = w;		pCurrentView->projection.m[0][1] = 0.0f;	pCurrentView->projection.m[0][2] = 0.0f;			pCurrentView->projection.m[0][3] = 0.0f;
	pCurrentView->projection.m[1][0] = 0.0f;	pCurrentView->projection.m[1][1] = h;		pCurrentView->projection.m[1][2] = 0.0f;			pCurrentView->projection.m[1][3] = 0.0f;
	pCurrentView->projection.m[2][0] = 0.0f;	pCurrentView->projection.m[2][1] = 0.0f;	pCurrentView->projection.m[2][2] = zf/(zf-zn);		pCurrentView->projection.m[2][3] = 1.0f;
	pCurrentView->projection.m[3][0] = 0.0f;	pCurrentView->projection.m[3][1] = 0.0f;	pCurrentView->projection.m[3][2] = -zn*zf/(zf-zn);	pCurrentView->projection.m[3][3] = 0.0f;
}

void MFView_SetOrtho(MFRect *pOrthoRect)
{
	CALLSTACK;

	pCurrentView->viewProjDirty = true;
	pCurrentView->isOrtho = true;

	if(pOrthoRect)
	{
		pCurrentView->orthoRect = *pOrthoRect;
	}

	// construct and apply ortho projection
	float l = pCurrentView->orthoRect.x;
	float r = pCurrentView->orthoRect.x + pCurrentView->orthoRect.width;
	float b = pCurrentView->orthoRect.y + pCurrentView->orthoRect.height;
	float t = pCurrentView->orthoRect.y;
	float zn = 0.0f;
	float zf = 1.0f;

	pCurrentView->projection.m[0][0] = 2.0f/(r-l);	pCurrentView->projection.m[0][1] = 0.0f;		pCurrentView->projection.m[0][2] = 0.0f;			pCurrentView->projection.m[0][3] = 0.0f;
	pCurrentView->projection.m[1][0] = 0.0f;		pCurrentView->projection.m[1][1] = 2.0f/(t-b);	pCurrentView->projection.m[1][2] = 0.0f;			pCurrentView->projection.m[1][3] = 0.0f;
	pCurrentView->projection.m[2][0] = 0.0f;		pCurrentView->projection.m[2][1] = 0.0f;		pCurrentView->projection.m[2][2] = 1.0f/(zf-zn);	pCurrentView->projection.m[2][3] = 0.0f;
	pCurrentView->projection.m[3][0] = (l+r)/(l-r);	pCurrentView->projection.m[3][1] = (t+b)/(b-t);	pCurrentView->projection.m[3][2] = zn/(zn-zf);		pCurrentView->projection.m[3][3] = 1.0f;
}

void MFView_GetOrtho(MFRect *pOrthoRect)
{
	*pOrthoRect = pCurrentView->orthoRect;
}

bool MFView_IsOrtho()
{
	return pCurrentView->isOrtho;
}

void MFView_SetCameraMatrix(const MFMatrix &cameraMatrix)
{
	pCurrentView->cameraMatrix = cameraMatrix;
	pCurrentView->viewDirty = true;
	pCurrentView->viewProjDirty = true;
}

const MFMatrix& MFView_GetCameraMatrix()
{
	return pCurrentView->cameraMatrix;
}

const MFMatrix& MFView_GetWorldToViewMatrix()
{
	if(pCurrentView->viewDirty)
	{
		pCurrentView->view.Inverse(pCurrentView->cameraMatrix);
		pCurrentView->viewDirty = false;
	}

	return pCurrentView->view;
}

const MFMatrix& MFView_GetViewToScreenMatrix()
{
	return pCurrentView->projection;
}

const MFMatrix& MFView_GetWorldToScreenMatrix()
{
	if(pCurrentView->viewProjDirty)
	{
		pCurrentView->viewProj.Multiply(MFView_GetWorldToViewMatrix(), pCurrentView->projection);
		pCurrentView->viewProjDirty = false;
	}

	return pCurrentView->viewProj;
}

MFMatrix* MFView_GetLocalToScreen(const MFMatrix& localToWorld, MFMatrix *pOutput)
{
	pOutput->Multiply(localToWorld, pCurrentView->view);
	pOutput->Multiply(pCurrentView->projection);

	return pOutput;
}

MFMatrix* MFView_GetLocalToView(const MFMatrix& localToWorld, MFMatrix *pOutput)
{
	pOutput->Multiply(localToWorld, pCurrentView->view);

	return pOutput;
}
