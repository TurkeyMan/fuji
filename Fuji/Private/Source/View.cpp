#include "Common.h"
#include "Display_Internal.h"
#include "View_Internal.h"
#include "System.h"

#include <math.h>

View View::defaultView;
MFRect View::defaultOrthoRect;

View *gpViewStack = NULL;
View *pCurrentView = NULL;

void View_InitModule()
{
	// allocate view stack
	gpViewStack = (View*)Heap_Alloc(sizeof(View) * gDefaults.view.maxViewsOnStack);

	// set default ortho rect
	View::defaultOrthoRect.x = gDefaults.view.orthoMinX;
	View::defaultOrthoRect.y = gDefaults.view.orthoMinY;
	View::defaultOrthoRect.width = gDefaults.view.orthoMaxX;
	View::defaultOrthoRect.height = gDefaults.view.orthoMaxY;

	// initialise default view
	memset(&View::defaultView, 0, sizeof(View));
	pCurrentView = &View::defaultView;

	pCurrentView->cameraMatrix = Matrix::identity;
	pCurrentView->view = Matrix::identity;
	pCurrentView->viewProj = Matrix::identity;

	View_SetOrtho(&View::defaultOrthoRect);
	View_SetProjection(DEGREES(gDefaults.view.defaultFOV), gDefaults.view.defaultAspect, gDefaults.view.defaultNearPlane, gDefaults.view.defaultFarPlane);

	pCurrentView = gpViewStack;
	View_SetDefault();
}

void View_DeinitModule()
{
	/// free view stack
	if(gpViewStack)
	{
		Heap_Free(gpViewStack);
		gpViewStack = NULL;
	}
}

void View_Push()
{
	DBGASSERT(pCurrentView - gpViewStack < (int)gDefaults.view.maxViewsOnStack, "Error: Exceeded maximum views on the stack. Increase 'gDefaults.view.maxViewsOnStack'.");

	// push view pointer foreward
	pCurrentView++;
	*pCurrentView = pCurrentView[-1];
}

void View_Pop()
{
	DBGASSERT(pCurrentView > gpViewStack, "Error: No views on the stack!");

	pCurrentView--;
}


void View_SetDefault()
{
	*pCurrentView = View::defaultView;
}

void View_SetProjection(float _fov, float _aspectRatio, float _nearPlane, float _farPlane)
{
	CALLSTACK;

	if(_fov)
		pCurrentView->fov = _fov;
	if(_nearPlane)
		pCurrentView->nearPlane = _nearPlane;
	if(_farPlane)
		pCurrentView->farPlane = _farPlane;
	if(_aspectRatio)
		pCurrentView->aspectRatio = _aspectRatio;

	pCurrentView->isOrtho = false;
	pCurrentView->viewProjDirty = true;

	// construct and apply perspective projection
	float zn = pCurrentView->nearPlane;
	float zf = pCurrentView->farPlane;

	float a = pCurrentView->fov * 0.5f;

	float h = MFCos(a) / MFSin(a);
	float w = h / pCurrentView->aspectRatio;

#if 1//!defined(_PSP)
	pCurrentView->projection.m[0][0] = w;		pCurrentView->projection.m[0][1] = 0.0f;	pCurrentView->projection.m[0][2] = 0.0f;			pCurrentView->projection.m[0][3] = 0.0f;
	pCurrentView->projection.m[1][0] = 0.0f;	pCurrentView->projection.m[1][1] = h;		pCurrentView->projection.m[1][2] = 0.0f;			pCurrentView->projection.m[1][3] = 0.0f;
	pCurrentView->projection.m[2][0] = 0.0f;	pCurrentView->projection.m[2][1] = 0.0f;	pCurrentView->projection.m[2][2] = zf/(zf-zn);		pCurrentView->projection.m[2][3] = 1.0f;
	pCurrentView->projection.m[3][0] = 0.0f;	pCurrentView->projection.m[3][1] = 0.0f;	pCurrentView->projection.m[3][2] = -zn*zf/(zf-zn);	pCurrentView->projection.m[3][3] = 0.0f;
#else
	pCurrentView->projection.m[0][0] = w;		pCurrentView->projection.m[0][1] = 0.0f;	pCurrentView->projection.m[0][2] = 0.0f;			pCurrentView->projection.m[0][3] = 0.0f;
	pCurrentView->projection.m[1][0] = 0.0f;	pCurrentView->projection.m[1][1] = h;		pCurrentView->projection.m[1][2] = 0.0f;			pCurrentView->projection.m[1][3] = 0.0f;
	pCurrentView->projection.m[2][0] = 0.0f;	pCurrentView->projection.m[2][1] = 0.0f;	pCurrentView->projection.m[2][2] = (zf+zn)/(zn-zf);	pCurrentView->projection.m[2][3] = -1.0f;
	pCurrentView->projection.m[3][0] = 0.0f;	pCurrentView->projection.m[3][1] = 0.0f;	pCurrentView->projection.m[3][2] = 2.0f*(zf*zn)/(zn-zf); pCurrentView->projection.m[3][3] = 0.0f;
#endif
}

void View_GetProjection(float *pFov, float *pAspectRatio, float *pNearPlane, float *pFarPlane)
{
	*pFov = pCurrentView->fov;
	*pAspectRatio = pCurrentView->aspectRatio;
	*pNearPlane = pCurrentView->nearPlane;
	*pFarPlane = pCurrentView->farPlane;
}

void View_SetOrtho(MFRect *pOrthoRect)
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

#if 1//!defined(_PSP)
	pCurrentView->projection.m[0][0] = 2.0f/(r-l);	pCurrentView->projection.m[0][1] = 0.0f;		pCurrentView->projection.m[0][2] = 0.0f;			pCurrentView->projection.m[0][3] = 0.0f;
	pCurrentView->projection.m[1][0] = 0.0f;		pCurrentView->projection.m[1][1] = 2.0f/(t-b);	pCurrentView->projection.m[1][2] = 0.0f;			pCurrentView->projection.m[1][3] = 0.0f;
	pCurrentView->projection.m[2][0] = 0.0f;		pCurrentView->projection.m[2][1] = 0.0f;		pCurrentView->projection.m[2][2] = 1.0f/(zf-zn);	pCurrentView->projection.m[2][3] = 0.0f;
	pCurrentView->projection.m[3][0] = (l+r)/(l-r);	pCurrentView->projection.m[3][1] = (t+b)/(b-t);	pCurrentView->projection.m[3][2] = zn/(zn-zf);		pCurrentView->projection.m[3][3] = 1.0f;
#else
	pCurrentView->projection.m[0][0] = 2.0f/(r-l);	pCurrentView->projection.m[0][1] = 0.0f;		pCurrentView->projection.m[0][2] = 0.0f;			pCurrentView->projection.m[0][3] = 0.0f;
	pCurrentView->projection.m[1][0] = 0.0f;		pCurrentView->projection.m[1][1] = 2.0f/(t-b);	pCurrentView->projection.m[1][2] = 0.0f;			pCurrentView->projection.m[1][3] = 0.0f;
	pCurrentView->projection.m[2][0] = 0.0f;		pCurrentView->projection.m[2][1] = 0.0f;		pCurrentView->projection.m[2][2] = 1.0f/(zn-zf);	pCurrentView->projection.m[2][3] = 0.0f;
	pCurrentView->projection.m[3][0] = (l+r)/(l-r);	pCurrentView->projection.m[3][1] = (t+b)/(b-t);	pCurrentView->projection.m[3][2] = zn/(zn-zf);		pCurrentView->projection.m[3][3] = 1.0f;
#endif
}

void View_GetOrtho(MFRect *pOrthoRect)
{
	*pOrthoRect = pCurrentView->orthoRect;
}

bool View_IsOrtho()
{
	return pCurrentView->isOrtho;
}

void View_SetCameraMatrix(const Matrix &cameraMatrix)
{
	pCurrentView->cameraMatrix = cameraMatrix;
	pCurrentView->viewDirty = true;
	pCurrentView->viewProjDirty = true;
}

const Matrix& View_GetCameraMatrix()
{
	return pCurrentView->cameraMatrix;
}

const Matrix& View_GetWorldToViewMatrix()
{
	if(pCurrentView->viewDirty)
	{
		pCurrentView->view.Inverse(pCurrentView->cameraMatrix);
		pCurrentView->viewDirty = false;
	}

	return pCurrentView->view;
}

const Matrix& View_GetViewToScreenMatrix()
{
	return pCurrentView->projection;
}

const Matrix& View_GetWorldToScreenMatrix()
{
	if(pCurrentView->viewProjDirty)
	{
		pCurrentView->viewProj.Multiply(View_GetWorldToViewMatrix(), pCurrentView->projection);
		pCurrentView->viewProjDirty = false;
	}

	return pCurrentView->viewProj;
}

Matrix* View_GetLocalToScreen(const Matrix& localToWorld, Matrix *pOutput)
{
	pOutput->Multiply(localToWorld, pCurrentView->view);
	pOutput->Multiply(pCurrentView->projection);

	return pOutput;
}

Matrix* View_GetLocalToView(const Matrix& localToWorld, Matrix *pOutput)
{
	pOutput->Multiply(localToWorld, pCurrentView->view);

	return pOutput;
}


// -------------------------------
/*
void View::SetProjection(float fov, float nearPlane, float farPlane)
{
  float ax, ay, az;     // x,y,z aspect ratio
  float cx, cy, cz;     // center of rendering
  float zmin, zmax;     // min and max Z-Buffer range
  Matrix temp;
 
  // set some default (magic) values for aspect and center
  fieldOfView = fov;
  distFromScreen = (float)320.0f / tanf(fov * 0.5f);
 
  ax = 1.0f * xPixelScale * xAspectRatio;
  ay = -1.0f * yPixelScale * yAspectRatio;
 
  cx = 2048.0f;
  cy = 2048.0f;
  zmin = 1.0f;
 
  if (gDisplay.zBufferPixFmt == PIXFMT_Z_24)
  {
    // must reduce Z Range since we mathematically errors
    zmax = 16777215.0f;
  }
  else
  {
    zmax = 65535.0f;
  }
 
 cz = (-zmax * nearPlane + zmin * farPlane) / (-nearPlane + farPlane);
 az  = farPlane * nearPlane * (-zmin + zmax) / (-nearPlane + farPlane);
 
 //     | scrz    0  0 0 |
 // m = |    0 scrz  0 0 |
 //     |    0    0  0 1 |
 //     |    0    0  1 0 |
  temp.SetIdentity();
 temp.m[0][0] = distFromScreen;
 temp.m[1][1] = distFromScreen;
 temp.m[2][2] = 0;
 temp.m[3][3] = 0;
 temp.m[3][2] = 1.0;
 temp.m[2][3] = 1.0;
 
 //     | ax  0  0  0 |
 // m = |  0 ay  0  0 |
 //     |  0  0 az  0 |
 //     | cx cy cz  1 |
  viewToScreenMatrix.SetIdentity();
  viewToScreenMatrix.m[0][0] = ax;
  viewToScreenMatrix.m[1][1] = ay;
  viewToScreenMatrix.m[2][2] = az;
  viewToScreenMatrix.m[3][0] = cx;
  viewToScreenMatrix.m[3][1] = cy;
  viewToScreenMatrix.m[3][2] = cz;
  viewToScreenMatrix.Multiply4x4(&temp, &viewToScreenMatrix);
 
  // calculate the transformation matrices
  worldToScreenMatrix.Multiply4x4(&worldToViewMatrix, &viewToScreenMatrix);
  localToScreenMatrix.Multiply4x4(&localToWorldMatrix, &worldToScreenMatrix);
 
  // store the clipping planes
  nearClipPlane = nearPlane;
  farClipPlane = farPlane;
//  farRejectPlane = 0.0f;
 
  //
  // Calculate the view-to-clip space and clip-to-screen space matrices
  //
 
  const float extra = 6.0f;
  float   gsx, gsy;
 
  float wideFov = atanf(extra * tanf(fov*0.5f)) * 2.0f;
 
  distFromScreen = (float)(320.0f*extra) / tanf(wideFov * 0.5f);
 

  gsx = extra*640.0f/2.0f  * nearPlane / distFromScreen / xAspectRatio;
  gsy = extra*512.0f/1.75f * nearPlane / distFromScreen / yAspectRatio;
 

  //              |2n/2gsx    0        0           0      |
  // viewToClip = |   0    2n/2gsy     0           0      |
  //              |   0       0   (f+n)/(f-n)      1      |
  //              |   0       0   -2f*n/(f-n)      0      |
  viewToClipMatrix.SetIdentity();
  viewToClipMatrix.m[0][0] = 2 * nearPlane / (2*gsx);
  viewToClipMatrix.m[1][1] = 2 * nearPlane / (2*gsy);
  viewToClipMatrix.m[2][2] = (farPlane + nearPlane) / (farPlane-nearPlane);
  viewToClipMatrix.m[3][3] = 0;
  viewToClipMatrix.m[3][2] = -2*(farPlane * nearPlane) / (farPlane-nearPlane);
  viewToClipMatrix.m[2][3] = 1;  
 
  //                |scrz*ax*gsx/n    0            0               0       |
  // clipToScreen = |   0        scrz*ay*gsy/n     0               0       |
  //                |   0             0      (-zmax+zmin)/2        0       |
  //                |   cx            cy      (zmin+zmax)/2        1       |
  clipToScreenMatrix.SetIdentity();
  clipToScreenMatrix.m[0][0] = distFromScreen*ax*gsx/nearPlane;
  clipToScreenMatrix.m[1][1] = distFromScreen*ay*gsy/nearPlane;
  clipToScreenMatrix.m[2][2] = (-zmax+zmin)/2;
  clipToScreenMatrix.m[3][3] = 1; 
  clipToScreenMatrix.m[3][0] = cx;
  clipToScreenMatrix.m[3][1] = cy;
  clipToScreenMatrix.m[3][2] = (zmax+zmin)/2;
 
#if 1
  for (int xx=0; xx<4; xx++)
  {
    for (int yy=0; yy<4; yy++)
    {
      viewToClipMatrix.m[xx][yy] *= W_SCALE;
    }
  }
#endif
 
  // TDL used for visibility culling.
  float f = 1.f/tanf(fov/2.f);
  UpdateViewFrustum(xAspectRatio*f, yAspectRatio*f*640.f/512.f);
  CalcMatrices();
}
 
 
bool View::SetOrtho(bool bActivate, float orthoWidth, float orthoHeight)
{
  CALLSTACK("View::SetOrtho");
 
 bool oldState = this->bIsOrtho;
 
 // If a change in ortho state is required then set
 if(this->bIsOrtho != bActivate)
 {
  if(bActivate)
  {
   // save projection matrix & view matrix
    orthoSaveWorldToView    = worldToViewMatrix;
    orthoSaveCamera         = cameraMatrix;
    orthoSaveViewToClip     = viewToClipMatrix;
    orthoSaveViewToScreen   = viewToScreenMatrix;
    orthoSaveClipToScreen   = clipToScreenMatrix;
   
    orthoSaveNearClipPlane  = nearClipPlane;
    orthoSaveFarClipPlane   = farClipPlane;
    nearClipPlane           = -10.0f;
    farClipPlane            = 10.0f;
   
    // set ortho matrix & identity view matrix
    Matrix m;
    m.SetIdentity();
    worldToViewMatrix = m;
      cameraMatrix = m;
     
    float orthoWidth = gDisplay.orthoWidth / xAspectRatio;
    float orthoHeight = gDisplay.orthoHeight / yAspectRatio;
 
    m.SetXAxis(2.0f/orthoWidth, 0.0f, 0.0f, 0.0f);
    m.SetYAxis(0.0f, -2.0f/orthoHeight, 0.0f, 0.0f);
    m.SetZAxis(0.0f, 0.0f, -1.0f, 0.0f);
    m.SetTranslation(-1.0f * xAspectRatio, 1.0f * yAspectRatio, 1.0f, 1.0f);
    viewToClipMatrix = m;
 
      //clipToScreenMatrix.m[0][0] = gDisplay.orthoWidth * (float)(clipMaxX - clipMinX + 1)/640.0f * 0.5f * ((pRenderTexture ? pRenderTexture->width : gDisplay.drawWidth) / gDisplay.orthoWidth);
      //clipToScreenMatrix.m[1][1] = -gDisplay.orthoHeight * (float)(clipMaxY - clipMinY + 1)/448.0f * 0.5f * ((pRenderTexture ? pRenderTexture->height : gDisplay.drawHeight) / gDisplay.orthoHeight);
    clipToScreenMatrix.m[0][0] = gDisplay.orthoWidth * (float)(clipMaxX - clipMinX + 1)/640.0f * 0.5f;//* ((pRenderTexture ? pRenderTexture->width : gDisplay.orthoWidth) / gDisplay.orthoWidth);
      clipToScreenMatrix.m[1][1] = -gDisplay.orthoHeight * (float)(clipMaxY - clipMinY + 1)/512.0f * 0.5f;// * ((pRenderTexture ? pRenderTexture->height : gDisplay.orthoHeight) / gDisplay.orthoHeight);
    clipToScreenMatrix.m[2][2] = 16777215.0f;
    clipToScreenMatrix.m[3][2] = 0.0f;
     
    viewToScreenMatrix.Multiply4x4(&viewToClipMatrix, &clipToScreenMatrix);
    this->bIsOrtho = true;
  }
  else
  {
   // restore projection matrix & view matrix
    worldToViewMatrix   = orthoSaveWorldToView;
    cameraMatrix        = orthoSaveCamera;
    viewToClipMatrix    = orthoSaveViewToClip;
    viewToScreenMatrix  = orthoSaveViewToScreen;
    clipToScreenMatrix  = orthoSaveClipToScreen;
    nearClipPlane       = orthoSaveNearClipPlane;
    farClipPlane        = orthoSaveFarClipPlane;
    this->bIsOrtho = false;
  }
 
  CalcMatrices();
 }
 
 return oldState;
}
 
 
//**********************************************************************
//* Function:     Init
//* Author:       James Podesta
//* Date:         23/01/2001
//* Description:  Init full screen viewport
//* Parameters:   void
//* Returns:      void
//**********************************************************************
void View::Init(float topLeftX, float topLeftY, float bottomRightX, float bottomRightY)
{
  MKVector3 pos, target;
  int width, height;
 
  this->topLeftX = topLeftX;
  this->topLeftY = topLeftY;
  this->bottomRightX = bottomRightX;
  this->bottomRightY = bottomRightY;
 
  int ps2topLeftX = (int)topLeftX * gDisplay.drawWidth / 640;
  int ps2topLeftY = (int)topLeftY * gDisplay.drawHeight / 512;
  int ps2bottomRightX = (int)bottomRightX * (gDisplay.drawWidth-1) / 640;
  int ps2bottomRightY = (int)bottomRightY * (gDisplay.drawHeight-1) / 512;
 
  width = ps2bottomRightX - ps2topLeftX + 1;
  height = ps2bottomRightY - ps2topLeftY + 1;
 
  pos.Set(0.0f, 0.0f, 0.0f);
  target.Set(0.0f, 0.0f, 1.0f);
 
  localToWorldMatrix.SetIdentity();
  worldToViewMatrix.SetIdentity();
  viewToScreenMatrix.SetIdentity();
 
  SetCameraLookAt(pos, target);
  SetProjection(Deg2Rad(90.0f), 0.5f, 1000.0f);
 
  SetLightDirection(0, (MKVector3 *)&defaultLightSetup[0]);
  SetLightDirection(1, (MKVector3 *)&defaultLightSetup[2]);
  SetLightDirection(2, (MKVector3 *)&defaultLightSetup[4]);
 
  SetLightColor(0, &defaultLightSetup[1]);
  SetLightColor(1, &defaultLightSetup[3]);
  SetLightColor(2, &defaultLightSetup[5]);
 
  SetAmbientColor(&defaultLightSetup[6]);
 
  localLightMatrix.SetIdentity();
 
  // default to full screen viewport with system render target settings
  clipMinX = (uint32)ps2topLeftX;
  clipMinY = (uint32)ps2topLeftY;
  clipMaxX = (uint32)ps2bottomRightX;
  clipMaxY = (uint32)ps2bottomRightY;
  renderOffsetX = (uint32)Fixed(2048 - (width>>1) - ps2topLeftX);
  renderOffsetY = (uint32)Fixed(2048 - (height>>1) - ps2topLeftY);
 
  xPixelScale = (float)(clipMaxX - clipMinX + 1) / 640.0f;
  yPixelScale = (float)(clipMaxY - clipMinY + 1) / 512.0f;
  xAspectRatio = 1.0f;
  yAspectRatio = 1.0f;
 
  pRenderTexture = NULL;
  pZBufferTexture = NULL;
  bDisableZWrite = false;
  bIsOrtho = false;
 
  minX = (float)(2048 - ((clipMaxX - clipMinX + 1)>>1));
  maxX = (float)(2048 + ((clipMaxX - clipMinX + 1)>>1) - 1);
  minY = (float)(2048 - ((clipMaxY - clipMinY + 1)>>1));
  maxY = (float)(2048 + ((clipMaxY - clipMinY + 1)>>1) - 1);
};
 
 
//**********************************************************************
//* Function:     SetRenderArea
//* Author:       James Podesta
//* Date:         23/01/2001
//* Description:  set the render area on the rendering target (screen/texture) in 640x512 coordinates
//* Parameters:   topLeftX
//*               topLeftY
//*               bottomRightX
//*               bottomRightY
//* Returns:      void
//**********************************************************************
 
void View::SetRenderArea(float topLeftX, float topLeftY, float bottomRightX, float bottomRightY)
{
  int width, height;
  int ps2topLeftX;
  int ps2topLeftY;
  int ps2bottomRightX;
  int ps2bottomRightY;
 
  this->topLeftX = topLeftX;
  this->topLeftY = topLeftY;
  this->bottomRightX = bottomRightX;
  this->bottomRightY = bottomRightY;
 
  if (pRenderTexture)
  {
    ps2topLeftX = (int)topLeftX * pRenderTexture->width / 640;
    ps2topLeftY = (int)topLeftY * pRenderTexture->height / 512;
    ps2bottomRightX = (int)bottomRightX * pRenderTexture->width / 640;
    ps2bottomRightY = (int)bottomRightY * pRenderTexture->height / 512;
  }
  else
  {
    ps2topLeftX = (int)topLeftX * gDisplay.drawWidth / 640;
    ps2topLeftY = (int)topLeftY * gDisplay.drawHeight / 512;
    ps2bottomRightX = (int)bottomRightX * gDisplay.drawWidth / 640;
    ps2bottomRightY = (int)bottomRightY * gDisplay.drawHeight / 512;
  }
 
  width = ps2bottomRightX - ps2topLeftX + 1;
  height = ps2bottomRightY - ps2topLeftY + 1;
 
  // default to full screen viewport with system render target settings
  clipMinX = (uint32)ps2topLeftX;
  clipMinY = (uint32)ps2topLeftY;
  clipMaxX = (uint32)ps2bottomRightX;
  clipMaxY = (uint32)ps2bottomRightY;
  renderOffsetX = (uint32)Fixed(2048 - (width>>1) - ps2topLeftX);
  renderOffsetY = (uint32)Fixed(2048 - (height>>1) - ps2topLeftY);
 
  xPixelScale = (float)(clipMaxX - clipMinX + 1) / 640.0f;
  yPixelScale = (float)(clipMaxY - clipMinY + 1) / 512.0f;
 
  minX = (float)(2048 - ((clipMaxX - clipMinX + 1)>>1));
  maxX = (float)(2048 + ((clipMaxX - clipMinX + 1)>>1) - 1);
  minY = (float)(2048 - ((clipMaxY - clipMinY + 1)>>1));
  maxY = (float)(2048 + ((clipMaxY - clipMinY + 1)>>1) - 1);
 
  SetProjection(fieldOfView, nearClipPlane, farClipPlane);
}
*/
