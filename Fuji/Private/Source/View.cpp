#include "Common.h"
#include "Display.h"
#include "View.h"

View View::defaultView;
View View::currentView;

View::View()
{
	*this = defaultView;
}

View* View::GetCurrent()
{
	return &currentView;
}

void View::UseDefault()
{
	currentView = defaultView;
}

void View::Use()
{
	currentView = *this;
}

void View::SetProjection(float fov)
{
	CALLSTACK;

	FOV = fov;
	isOrtho = false;
	viewProjDirty = true;

	// construct and apply perspective projection
	D3DXMatrixPerspectiveFovLH((D3DXMATRIX*)&projection, fov, (display.wide ? WIDE_ASPECT : STANDARD_ASPECT), 0.1f, 10000.0f);
}

bool View::SetOrtho(bool enabled, float width, float height)
{
	CALLSTACK;

	bool t = isOrtho;
	isOrtho = enabled;

	if(enabled)
	{
		float extend = 0.0f;
		viewProjDirty = true;

		// correct for widescreen
		if(display.wide) extend = (((width/1.333333333f)*1.77777777778f)-width)/2.0f;

		// construct and apply ortho projection
		D3DXMatrixOrthoOffCenterLH((D3DXMATRIX*)&projection, -extend, width + extend, height, 0, 0.0f, 1000.0f);
	}
	else
	{
		SetProjection(FOV);
	}

	return t;
}

void View::SetCameraMatrix(const Matrix &viewMat)
{
	view = viewMat;
	viewProjDirty = true;
}

Matrix* View::GetWorldToScreenMatrix()
{
	if(viewProjDirty)
	{
		viewProj.Multiply(view, projection);
		viewProjDirty = false;
	}

	return &viewProj;
}

Matrix *View::GetLocalToScreen(const Matrix& localToWorld, Matrix *pOutput)
{
	pOutput->Multiply(localToWorld, view);
	pOutput->Multiply(projection);

	return pOutput;
}

Matrix *View::GetLocalToView(const Matrix& localToWorld, Matrix *pOutput)
{
	pOutput->Multiply(localToWorld, view);

	return pOutput;
}

