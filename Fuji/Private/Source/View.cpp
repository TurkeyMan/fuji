#include "Common.h"
#include "Display.h"
#include "View.h"

#include <math.h>

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
	float zn = 0.1f;
	float zf = 10000.0f;

	float a = fov * 0.5f;

	float h = cosf(a) / sinf(a);
	float w = h / (display.wide ? WIDE_ASPECT : STANDARD_ASPECT);

	projection.m[0][0] = w;		projection.m[0][1] = 0.0f;	projection.m[0][2] = 0.0f;				projection.m[0][3] = 0.0f;
	projection.m[1][0] = 0.0f;	projection.m[1][1] = h;		projection.m[1][2] = 0.0f;				projection.m[1][3] = 0.0f;
	projection.m[2][0] = 0.0f;	projection.m[2][1] = 0.0f;	projection.m[2][2] = zf/(zf-zn);		projection.m[2][3] = 1.0f;
	projection.m[3][0] = 0.0f;	projection.m[3][1] = 0.0f;	projection.m[3][2] = -zn*zf/(zf-zn);	projection.m[3][3] = 0.0f;
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
		float l = -extend;
		float r = width + extend;
		float b = height;
		float t = 0.0f;
		float zn = 0.0f;
		float zf = 1000.0f;

		projection.m[0][0] = 2.0f/(r-l);	projection.m[0][1] = 0.0f;			projection.m[0][2] = 0.0f;			projection.m[0][3] = 0.0f;
		projection.m[1][0] = 0.0f;			projection.m[1][1] = 2.0f/(t-b);	projection.m[1][2] = 0.0f;			projection.m[1][3] = 0.0f;
		projection.m[2][0] = 0.0f;			projection.m[2][1] = 0.0f;			projection.m[2][2] = 1.0f/(zf-zn);	projection.m[2][3] = 0.0f;
		projection.m[3][0] = (l+r)/(l-r);	projection.m[3][1] = (t+b)/(b-t);	projection.m[3][2] = zn/(zn-zf);	projection.m[3][3] = 1.0f;
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

