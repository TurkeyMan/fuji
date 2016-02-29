#include "Fuji_Internal.h"
#include "MFHeap.h"
#include "MFDisplay_Internal.h"
#include "MFView_Internal.h"
#include "MFSystem.h"
#include "MFRenderState.h"

#include <math.h>

MFView MFView::defaultView;
MFRect MFView::defaultOrthoRect;

MFView *gpViewStack = NULL;
MFView *pCurrentView = NULL;

MFInitStatus MFView_InitModule(int moduleId, bool bPerformInitialisation)
{
	// allocate view stack
	gpViewStack = (MFView*)MFHeap_Alloc(sizeof(MFView) * gDefaults.view.maxViewsOnStack);

	// set default ortho rect
	MFView::defaultOrthoRect.x = gDefaults.view.orthoMinX;
	MFView::defaultOrthoRect.y = gDefaults.view.orthoMinY;
	MFView::defaultOrthoRect.width = gDefaults.view.orthoMaxX;
	MFView::defaultOrthoRect.height = gDefaults.view.orthoMaxY;

	// initialise default view
	MFZeroMemory(&MFView::defaultView, sizeof(MFView));
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

	return MFIS_Succeeded;
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

MF_API void MFView_Push()
{
	MFDebug_Assert(pCurrentView - gpViewStack < (int)gDefaults.view.maxViewsOnStack, "Error: Exceeded maximum views on the stack. Increase 'gDefaults.view.maxViewsOnStack'.");

	// push view pointer forward
	pCurrentView++;
	*pCurrentView = pCurrentView[-1];
}

MF_API void MFView_Pop()
{
	MFDebug_Assert(pCurrentView > gpViewStack, "Error: No views on the stack!");

	pCurrentView--;
}


MF_API void MFView_SetDefault()
{
	*pCurrentView = MFView::defaultView;
}

MF_API void MFView_ConfigureProjection(float fieldOfView, float nearPlane, float farPlane)
{
	pCurrentView->fov = fieldOfView;
	pCurrentView->nearPlane = nearPlane;
	pCurrentView->farPlane = farPlane;

	if(!pCurrentView->isOrtho)
	{
		pCurrentView->viewProjDirty = true;
		pCurrentView->projDirty = true;

		pCurrentView->pStateBlock = NULL;
	}
}

MF_API void MFView_SetCustomProjection(const MFMatrix &projectionMatrix, bool bYIsUp)
{
	pCurrentView->projection = projectionMatrix;

	pCurrentView->isOrtho = !bYIsUp;
	pCurrentView->customProjection = true;
	pCurrentView->projDirty = false;
	pCurrentView->viewProjDirty = true;

	pCurrentView->pStateBlock = NULL;
}

MF_API void MFView_SetAspectRatio(float aspectRatio)
{
	pCurrentView->aspectRatio = aspectRatio;

	if(!pCurrentView->isOrtho)
	{
		pCurrentView->viewProjDirty = true;
		pCurrentView->projDirty = true;

		pCurrentView->pStateBlock = NULL;
	}
}

MF_API void MFView_SetProjection()
{
	MFCALLSTACK;

	if(pCurrentView->customProjection || pCurrentView->isOrtho)
	{
		pCurrentView->isOrtho = false;
		pCurrentView->viewDirty = true;
		pCurrentView->viewProjDirty = true;
		pCurrentView->projDirty = true;
		pCurrentView->customProjection = false;

		pCurrentView->pStateBlock = NULL;
	}
}

MF_API void MFView_SetOrtho(const MFRect *pOrthoRect)
{
	MFCALLSTACK;

	if(pCurrentView->customProjection || !pCurrentView->isOrtho || pOrthoRect)
	{
		pCurrentView->isOrtho = true;
		pCurrentView->viewDirty = true;
		pCurrentView->viewProjDirty = true;
		pCurrentView->projDirty = true;
		pCurrentView->customProjection = false;

		pCurrentView->pStateBlock = NULL;

		if(pOrthoRect)
			pCurrentView->orthoRect = *pOrthoRect;
	}
}

MF_API void MFView_GetOrthoRect(MFRect *pOrthoRect)
{
	*pOrthoRect = pCurrentView->orthoRect;
}

MF_API bool MFView_IsOrtho()
{
	return pCurrentView->isOrtho;
}

MF_API void MFView_SetViewport(const MFRect *pViewport)
{
	if (pViewport)
	{
		pCurrentView->viewport = *pViewport;
		pCurrentView->viewportSet = true;
	}
	else
		pCurrentView->viewportSet = false;
}

MF_API void MFView_GetViewport(MFRect *pViewport)
{
	if (pCurrentView->viewportSet)
		*pViewport = pCurrentView->viewport;
	else
		MFDisplay_GetDisplayRect(pViewport);
}

MF_API void MFView_SetCameraMatrix(const MFMatrix &cameraMatrix)
{
	pCurrentView->cameraMatrix = cameraMatrix;

	pCurrentView->viewDirty = true;
	pCurrentView->viewProjDirty = true;

	pCurrentView->pStateBlock = NULL;
}

MF_API const MFMatrix& MFView_GetCameraMatrix()
{
	return pCurrentView->cameraMatrix;
}

MF_API const MFStateBlock* MFView_GetViewState()
{
	if(!pCurrentView->pStateBlock)
	{
		pCurrentView->pStateBlock = MFStateBlock_CreateTemporary(256);
		MFStateBlock_SetMatrix(pCurrentView->pStateBlock, MFSCM_Projection, MFView_GetViewToScreenMatrix());
		MFStateBlock_SetMatrix(pCurrentView->pStateBlock, MFSCM_Camera, pCurrentView->isOrtho ? MFMatrix::identity : pCurrentView->cameraMatrix);
		if (pCurrentView->viewportSet)
			MFStateBlock_SetViewport(pCurrentView->pStateBlock, pCurrentView->viewport);
	}

	return pCurrentView->pStateBlock;
}

MF_API const MFMatrix& MFView_GetWorldToViewMatrix()
{
	if(pCurrentView->viewDirty)
	{
		if(pCurrentView->isOrtho)
			pCurrentView->view.SetIdentity();
		else
			pCurrentView->view.Inverse(pCurrentView->cameraMatrix);
		pCurrentView->viewDirty = false;
	}

	return pCurrentView->view;
}

MF_API const MFMatrix& MFView_GetViewToScreenMatrix()
{
	if(pCurrentView->projDirty)
	{
		if(pCurrentView->isOrtho)
			pCurrentView->projection.SetOrthographic(pCurrentView->orthoRect, 0.f, 1.f);
		else
			pCurrentView->projection.SetPerspective(pCurrentView->fov, pCurrentView->nearPlane, pCurrentView->farPlane, pCurrentView->aspectRatio);
		pCurrentView->projDirty = false;
	}

	return pCurrentView->projection;
}

MF_API const MFMatrix& MFView_GetWorldToScreenMatrix()
{
	if(pCurrentView->viewProjDirty)
	{
		pCurrentView->viewProj.Multiply4x4(MFView_GetWorldToViewMatrix(), MFView_GetViewToScreenMatrix());
		pCurrentView->viewProjDirty = false;
	}

	return pCurrentView->viewProj;
}

MF_API MFMatrix* MFView_GetLocalToScreen(const MFMatrix& localToWorld, MFMatrix *pOutput)
{
	pOutput->Multiply4x4(localToWorld, MFView_GetWorldToScreenMatrix());

	return pOutput;
}

MF_API MFMatrix* MFView_GetLocalToView(const MFMatrix& localToWorld, MFMatrix *pOutput)
{
	pOutput->Multiply4x4(localToWorld, MFView_GetWorldToViewMatrix());

	return pOutput;
}

MF_API void MFView_TransformPoint3DTo2D(const MFVector& point, MFVector *pResult)
{
	MFMatrix proj, viewProj, view;

	// get the perspective projection matrix
	proj.SetPerspective(pCurrentView->fov, pCurrentView->nearPlane, pCurrentView->farPlane, pCurrentView->aspectRatio);

	// in this special case, we'll make the projection matrix produce a 0-1 value in z across all platforms (some platforms project into different 'z' spaces)
	float zn = pCurrentView->nearPlane;
	float zf = pCurrentView->farPlane;
	float zd = zf-zn;
	float zs = zf/zd;
	proj.m[10] = zs;
	proj.m[14] = -zn*zs;

	// get the view matrix (which we will need to calculate if we are in ortho mode)
	if(!MFView_IsOrtho())
		view = MFView_GetWorldToViewMatrix();
	else
		view.Inverse(MFView_GetCameraMatrix());

	viewProj.Multiply4x4(view, proj);

	// apply the projection and perform the perspective divide
	MFVector transformed;
	transformed = ApplyMatrix(MakeVector(point, 1), viewProj);
	transformed *= MFRcp(transformed.w);

	// and shift the result into the ortho rect
	transformed.x += 1.0f;
	transformed.y = -transformed.y + 1.0f;

	*pResult = transformed * MakeVector(pCurrentView->orthoRect.width*0.5f, pCurrentView->orthoRect.height*0.5f) + MakeVector(pCurrentView->orthoRect.x, pCurrentView->orthoRect.y);
}

MF_API void MFView_TransformPoint2DTo3D(const MFVector& point, MFVector *pResult, MFVector *pResultRayDir)
{
	MFMatrix proj, viewProj, view;

	// get the perspective projection matrix
	proj.SetPerspective(pCurrentView->fov, pCurrentView->nearPlane, pCurrentView->farPlane, pCurrentView->aspectRatio);

	// in this special case, we'll make the projection matrix produce a 0-1 value in z across all platforms (some platforms project into different 'z' spaces)
	float zn = pCurrentView->nearPlane;
	float zf = pCurrentView->farPlane;
	float zd = zf-zn;
	float zs = zf/zd;
	proj.m[10] = zs;
	proj.m[14] = -zn*zs;

	// get the view matrix (which we will need to calculate if we are in ortho mode)
	if(!MFView_IsOrtho())
		view = MFView_GetWorldToViewMatrix();
	else
		view.Inverse(MFView_GetCameraMatrix());

	viewProj.Multiply4x4(view, proj);

	// inverse projection
	viewProj.Inverse();

	// which the point from ortho space back into homogeneous space
	*pResult = point;
	*pResult -= MakeVector(pCurrentView->orthoRect.x, pCurrentView->orthoRect.y);
	*pResult *= MakeVector(MFRcp(pCurrentView->orthoRect.width*0.5f), MFRcp(pCurrentView->orthoRect.height*0.5f));
	pResult->x -= 1.0f;
	pResult->y = -pResult->y + 1.0f;

	// and un-project
	// TODO: undo the perspective divide (fuck)
	*pResult = ApplyMatrix(*pResult, viewProj);

	if(pResultRayDir)
	{
		// calculate the pixels rays direction..

	}
}
