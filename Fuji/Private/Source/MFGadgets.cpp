#include "Common.h"
#include "MFPrimitive.h"
#include "MFWidgets.h"
#include "View.h"
#include "Font.h"
#include "Input.h"

#include <string.h>


MKWidgetAttributes MKWidgetAttributes::identityAttributes = { MKVector3::zero, MKVector3::zero, MKVector3::zero, MKVector4::one, 1.0f, 0 };
MKWidgetRenderContext MKWidgetRenderContext::identity = { Matrix::identity, MKVector4::one };

MKWidgetDescriptor *pDescriptorList = NULL;
MKWidgetSystem widgetSystem;


// MKWidgetDescriptor
void MKWidget_AddToDescriptorList(MKWidgetDescriptor *pDescriptor)
{
  pDescriptor->pNext = pDescriptorList;
  pDescriptorList = pDescriptor;
}


MKWidgetDescriptor::MKWidgetDescriptor(const char *name, MKWidgetDescriptor *parent, MKWidgetFactoryFunc creator)
{
  pName = name;
  pBase = parent;
  CreateInstance = creator;

  MKWidget_AddToDescriptorList(this);
}

// the factory func
MKWidget* MKWidget_FactoryFunc()
{
  return Heap_New(MKWidget);
}

MKWidget* MKWidgetContainer_FactoryFunc()
{
  return Heap_New(MKWidgetContainer);
}

MKWidget* MKWidgetFrame_FactoryFunc()
{
  return Heap_New(MKWidgetFrame);
}

MKWidget* MKWidgetWindow_FactoryFunc()
{
  return Heap_New(MKWidgetWindow);
}

MKWidget* MKWidgetSystem_FactoryFunc()
{
  return Heap_New(MKWidgetSystem);
}

MKWidget* MKWidgetWindowTitleBar_FactoryFunc()
{
  return Heap_New(MKWidgetWindowTitleBar);
}

MKWidget* MKWidgetButton_FactoryFunc()
{
  return Heap_New(MKWidgetButton);
}

MKWidget* MKWidgetString_FactoryFunc()
{
  return Heap_New(MKWidgetString);
}

MKWidget* MKWidgetCheckMark_FactoryFunc()
{
  return Heap_New(MKWidgetCheckMark);
}

MKWidget* MKWidgetCheckBox_FactoryFunc()
{
  return Heap_New(MKWidgetCheckBox);
}

MKWidget* MKWidgetSlider_FactoryFunc()
{
  return Heap_New(MKWidgetSlider);
}

MKWidget* MKWidgetScrollBar_FactoryFunc()
{
  return Heap_New(MKWidgetScrollBar);
}

MKWidget* MKWidgetThumb_FactoryFunc()
{
  return Heap_New(MKWidgetThumb);
}

MKWidgetDescriptor gMKWidgetDescriptor("MKWidget", NULL, MKWidget_FactoryFunc);
MKWidgetDescriptor gMKWidgetContainerDescriptor("MKWidgetContainer", &gMKWidgetDescriptor, MKWidgetContainer_FactoryFunc);
MKWidgetDescriptor gMKWidgetFrameDescriptor("MKWidgetFrame", &gMKWidgetContainerDescriptor, MKWidgetFrame_FactoryFunc);
MKWidgetDescriptor gMKWidgetWindowDescriptor("MKWidgetWindow", &gMKWidgetFrameDescriptor, MKWidgetWindow_FactoryFunc);
MKWidgetDescriptor gMKWidgetButtonDescriptor("MKWidgetButton", &gMKWidgetDescriptor, MKWidgetButton_FactoryFunc);
MKWidgetDescriptor gMKWidgetStringDescriptor("MKWidgetString", &gMKWidgetDescriptor, MKWidgetString_FactoryFunc);
MKWidgetDescriptor gMKWidgetCheckMarkDescriptor("MKWidgetCheckMark", &gMKWidgetDescriptor, MKWidgetCheckMark_FactoryFunc);
MKWidgetDescriptor gMKWidgetCheckBoxDescriptor("MKWidgetCheckBox", &gMKWidgetDescriptor, MKWidgetCheckBox_FactoryFunc);
MKWidgetDescriptor gMKWidgetSliderDescriptor("MKWidgetSlider", &gMKWidgetDescriptor, MKWidgetSlider_FactoryFunc);
MKWidgetDescriptor gMKWidgetScrollBarDescriptor("MKWidgetScrollBar", &gMKWidgetDescriptor, MKWidgetScrollBar_FactoryFunc);
MKWidgetDescriptor gMKWidgetThumbDescriptor("MKWidgetThumb", &gMKWidgetDescriptor, MKWidgetThumb_FactoryFunc);

MKWidgetDescriptor gMKWidgetSystemDescriptor("MKWidgetSystem", &gMKWidgetContainerDescriptor, MKWidgetSystem_FactoryFunc);
MKWidgetDescriptor gMKWidgetWindowTitleBarDescriptor("MKWidgetWindowTitleBar", &gMKWidgetDescriptor, MKWidgetWindowTitleBar_FactoryFunc);


// Internal functions

void MKWidget_InitModule()
{
  widgetSystem.attributes = MKWidgetAttributes::identityAttributes;
  widgetSystem.attributes.dimensions.x = 640.0f;
  widgetSystem.attributes.dimensions.y = 512.0f;
}

void MKWidget_DeinitModule()
{

}

void MKWidget_Update()
{
  widgetSystem.Update();
}

void MKWidget_Draw()
{
  bool bOrtho = View::GetCurrent()->SetOrtho(true);

  widgetSystem.Draw(MKWidgetRenderContext::identity);

  View::GetCurrent()->SetOrtho(bOrtho);
}

void WidgetDrawBevel(const MKVector3& topLeft, const MKVector3& bottomRight, float width, const MKVector4& hiliteColour, const MKVector4& shadowColour, bool sunken = false, bool client = false)
{
  MKBegin(14);

  MKSetColour(sunken ? shadowColour : hiliteColour);

  MKSetPosition(topLeft.x, bottomRight.y, 0.0f);
  MKSetPosition(topLeft.x+width, bottomRight.y-width, 0.0f);
  MKSetPosition(topLeft.x, topLeft.y, 0.0f);
  MKSetPosition(topLeft.x+width, topLeft.y+width, 0.0f);
  MKSetPosition(bottomRight.x, topLeft.y, 0.0f);
  MKSetPosition(bottomRight.x-width, topLeft.y+width, 0.0f);

  MKSetPosition(bottomRight.x-width, topLeft.y+width, 0.0f);
  MKSetPosition(topLeft.x, bottomRight.y, 0.0f);

  MKSetColour(sunken ? hiliteColour : shadowColour);

  MKSetPosition(topLeft.x, bottomRight.y, 0.0f);
  MKSetPosition(topLeft.x+width, bottomRight.y-width, 0.0f);
  MKSetPosition(bottomRight.x, bottomRight.y, 0.0f);
  MKSetPosition(bottomRight.x-width, bottomRight.y-width, 0.0f);
  MKSetPosition(bottomRight.x, topLeft.y, 0.0f);
  MKSetPosition(bottomRight.x-width, topLeft.y+width, 0.0f);

  MKEnd();

  if(client)
  {
    WidgetDrawBevel(topLeft+MKVector(width, width, 0.0f), bottomRight+MKVector(-width, -width, 0.0f), width, hiliteColour, shadowColour, !sunken);
  }
}

void WidgetDrawSquare(const MKVector3& topLeft, const MKVector3& bottomRight, float width, const MKVector4& colour)
{
  MKBegin(10);

  MKSetColour(colour);

  MKSetPosition(topLeft.x, bottomRight.y, 0.0f);
  MKSetPosition(topLeft.x+width, bottomRight.y-width, 0.0f);
  MKSetPosition(topLeft.x, topLeft.y, 0.0f);
  MKSetPosition(topLeft.x+width, topLeft.y+width, 0.0f);
  MKSetPosition(bottomRight.x, topLeft.y, 0.0f);
  MKSetPosition(bottomRight.x-width, topLeft.y+width, 0.0f);
  MKSetPosition(bottomRight.x, bottomRight.y, 0.0f);
  MKSetPosition(bottomRight.x-width, bottomRight.y-width, 0.0f);
  MKSetPosition(topLeft.x, bottomRight.y, 0.0f);
  MKSetPosition(topLeft.x+width, bottomRight.y-width, 0.0f);

  MKEnd();
}

void WidgetDrawBox(const MKVector3& topLeft, const MKVector3& bottomRight, const MKVector4& colour)
{
  MKBegin(4);

  MKSetColour(colour);
  MKSetPosition(topLeft);
  MKSetPosition(topLeft.x, bottomRight.y, 0.0f);
  MKSetPosition(bottomRight.x, topLeft.y, 0.0f);
  MKSetPosition(bottomRight);

  MKEnd();
}

/*** MKWidget Interface Functions ***/

// Create/Destroy
MKWidget* MKWidget_Create(const char *pWidgetType, const char *pName, const MKWidgetAttributes *pAttributes, MKWidget *pParent, MKWidgetMessageHook pMessageHook, void *pUserData)
{
  if(pParent) DBGASSERT(pParent->IsContainer(), "Parent Widget is not a container!");
  if(!pParent) pParent = &widgetSystem;

  for(MKWidgetDescriptor *pCurrent = pDescriptorList; pCurrent; pCurrent = pCurrent->pNext)
  {
    if(!stricmp(pCurrent->pName, pWidgetType))
    {
      MKWidget *pNew = pCurrent->CreateInstance();

      if(pAttributes)
        pNew->attributes = *pAttributes;

      pNew->pDescriptor = pCurrent;
      pNew->pMessageHook = pMessageHook;
      pNew->pUserData = pUserData;

      MKWidget_SendMessage(pParent, MKWM_ADDCHILD, (uint32)pNew);

      MKWidget_SendMessage(pNew, MKWM_CONSTRUCT, 0);

      // move and initialise the widget
      MKWidget_SetName(pNew, pName);
      MKWidget_Move(pNew, pAttributes->position);
      MKWidget_Resize(pNew, pAttributes->dimensions);
      MKWidget_Scale(pNew, pAttributes->scale);
      MKWidget_SetColour(pNew, pAttributes->colour);

      MKWidget_SendMessage(pNew, MKWM_CREATE, 0);

      return pNew;
    }
  }

  return NULL;
}

int MKWidget_Destroy(MKWidget *pWidget)
{
  return MKWidget_SendMessage(pWidget, MKWM_DESTROY, 0);
}

//Send a message to a Widget:
int MKWidget_SendMessage(MKWidget *pWidget, uint32 message, uint32 messageData)
{
  if(pWidget->pMessageHook)
  {
    int r = pWidget->pMessageHook(pWidget, message, messageData, pWidget->pUserData);

    if(r == 0) return 0;
  }

  return pWidget->MessageHandler(message, messageData);
}

//Finding widgets in the system.
MKWidget* MKWidget_Find(const char *pName, MKWidgetContainer *pParent)
{
  return NULL;
}

MKWidgetContainer* MKWidget_GetParent(MKWidget *pWidget)
{
  return NULL;
}

const char* MKWidget_GetName(MKWidget *pWidget)
{
  return NULL;
}


// Get/Set name
void MKWidget_SetName(MKWidget *pWidget, const char *pName)
{
  MKWidget_SendMessage(pWidget, MKWM_SETNAME, (uint32)pName);
}

const char* MKWidget_SetName(MKWidget *pWidget)
{
  char *pName;
  MKWidget_SendMessage(pWidget, MKWM_GETNAME, (uint32)&pName);
  return pName;
}


//Move/resize methods:
int MKWidget_Move(MKWidget *pWidget, const MKVector3& position)
{
  return MKWidget_SendMessage(pWidget, MKWM_MOVE, (uint32)&position);
}

int MKWidget_Resize(MKWidget *pWidget, const MKVector3& dimensions)
{
  return MKWidget_SendMessage(pWidget, MKWM_SIZE, (uint32)&dimensions);
}

int MKWidget_Scale(MKWidget *pWidget, float scale)
{
  return MKWidget_SendMessage(pWidget, MKWM_SCALE, (uint32)&scale);
}

int MKWidget_SetColour(MKWidget *pWidget, const MKVector4& colour)
{
  return MKWidget_SendMessage(pWidget, MKWM_SETCOLOUR, (uint32)&colour);
}


//Show/Enable:
int MKWidget_Show(MKWidget *pWidget, int show)
{
  return MKWidget_SendMessage(pWidget, MKWM_SHOW, show);
}

int MKWidget_Enable(MKWidget *pWidget, bool enable)
{
  return MKWidget_SendMessage(pWidget, MKWM_ENABLE, enable);
}


//For managing input focus (each return previous focus):
MKWidget* MKWidget_SetFocus(MKWidget *pWidget)
{
  MKWidget *pOld = widgetSystem.pFocus;

  if(MKWidget_SendMessage(pWidget, MKWM_SETFOCUS, 1))
  {
    MKWidget_SendMessage(pWidget, MKWM_SETFOCUS, 0);
  }

  return pOld;
}

MKWidget* MKWidget_SetMouseFocus(MKWidget *pWidget)
{
  MKWidget *pOld = widgetSystem.pMouseFocus;

  if(MKWidget_SendMessage(pWidget, MKWM_SETMOUSEFOCUS, 1))
  {
    MKWidget_SendMessage(pWidget, MKWM_SETMOUSEFOCUS, 0);
  }

  return pOld;
}

MKWidget* MKWidget_ReleaseMouseFocus()
{
  return widgetSystem.ChangeMouseFocus(NULL);
}

void MKWidget_GetMousePos(MKVector3 *pMousePos)
{
  pMousePos->x = (float)Input_GetButtonState(IDEV_MOUSE, MOUSE_POSX);
  pMousePos->y = (float)Input_GetButtonState(IDEV_MOUSE, MOUSE_POSY);
}

void MKWidget_GetLocalMousePos(MKWidget *pWidget, MKVector3 *pMousePos)
{
  pMousePos->x = (float)Input_GetButtonState(IDEV_MOUSE, MOUSE_POSX);
  pMousePos->y = (float)Input_GetButtonState(IDEV_MOUSE, MOUSE_POSY);
  pMousePos->z = 0.0f;

  Matrix mouseToWidget;
  mouseToWidget.SetIdentity();

  pWidget->GetLocalToWorldMatrix(&mouseToWidget);
  mouseToWidget.Inverse();

  ApplyMatrix(*pMousePos, *pMousePos, &mouseToWidget);
}


//Used to register a custom message handler to be linked up during creation of data driven source data.
void MKWidget_RegisterCustomMessageHandler(const char *pName, MKWidgetMessageHook pMessageHook)
{

}

//Load/Store:
void MKWidget_LoadWidgetFile(const char *pFilename, MKWidgetContainer *pParent)
{

}

void MKWidget_SaveWidgetFile(const char *pFilename, MKWidget *pWidget)
{

}

// MKWidget
MKWidget::MKWidget()
{
  pDescriptor = &gMKWidgetDescriptor;
  pMessageHook = NULL;
  pUserData = NULL;
  pParent = NULL;
  pNextSibling = NULL;
  pName = NULL;
  widgetFlags = WF_Enabled|WF_Visible;

  attributes = MKWidgetAttributes::identityAttributes;
}

int MKWidget::MessageHandler(uint32 message, uint32 data)
{
  switch(message)
  {
    case MKWM_CONSTRUCT:
      break;

    case MKWM_CREATE:
      break;

    case MKWM_DESTROY:
      Heap_Delete(this);
      break;

    case MKWM_MOVE:
      attributes.position = *(MKVector3*)data;
      break;

    case MKWM_SIZE:
      attributes.dimensions = *(MKVector3*)data;
      break;

    case MKWM_SCALE:
      attributes.scale = *(float*)data;
      break;

    case MKWM_SETCOLOUR:
      attributes.colour = *(MKVector4*)data;
     break;

    case MKWM_GETPOS:
      *(MKVector3*)data = attributes.position;
      break;

    case MKWM_GETSIZE:
      *(MKVector3*)data = attributes.dimensions;
      break;

    case MKWM_GETSCALE:
      *(float*)data = attributes.scale;
      break;

    case MKWM_GETCOLOUR:
      *(MKVector4*)data = attributes.colour;
      break;

    case MKWM_GETBOUNDINGVOLUME:
      ((MKWidgetBoundingVolume*)data)->position = attributes.position;
      ((MKWidgetBoundingVolume*)data)->dimensions = attributes.dimensions;
      break;

    case MKWM_GETCLIENTVOLUME:
      ((MKWidgetBoundingVolume*)data)->position = MKVector3::zero;
      ((MKWidgetBoundingVolume*)data)->dimensions = attributes.dimensions;
      break;

    case MKWM_GETNAME:
      *(const char**)data = pName;
      break;

    case MKWM_SETNAME:
      pName = (const char*)data;
      break;

    case MKWM_SHOW:
      widgetFlags = (widgetFlags & ~WF_Visible) | (data ? WF_Visible : 0);
      break;

    case MKWM_ENABLE:
      widgetFlags = (widgetFlags & ~WF_Enabled) | (data ? WF_Enabled : 0);
      break;

    case MKWM_SETFOCUS:
      if(data)
      {
        widgetSystem.ChangeFocus(this);
        return 1;
      }
      break;

    case MKWM_SETMOUSEFOCUS:
      if(data)
      {
        widgetSystem.ChangeMouseFocus(this);
        return 1;
      }
      break;

    case MKWM_MOUSEDOWN:
      MKWidget_SetFocus(this);
      break;

    case MKWM_NOTIFY:
      if(pParent) MKWidget_SendMessage(pParent, message, data);
      break;
  }

  return 0;
}

void MKWidget::Render(MKWidgetRenderContext *pRenderContext)
{
  // fix up render context
  Matrix attributeMatrix;

  attributeMatrix.SetIdentity();
  attributeMatrix.SetRotationPYR(attributes.rotation);
  attributeMatrix.Scale3x3(attributes.scale);
  attributeMatrix.SetTranslation(attributes.position.x, attributes.position.y, attributes.position.z);

  MKWidgetBoundingVolume client;
  MKWidget_SendMessage(this, MKWM_GETCLIENTVOLUME, (uint32)&client);
  attributeMatrix.Translate(client.position);

  pRenderContext->tramsform.Multiply4x4(&attributeMatrix);
  pRenderContext->colour *= attributes.colour;
}

void MKWidget::Draw(MKWidgetRenderContext renderContext)
{
  if(!(widgetFlags&WF_Visible)) return;

  Render(&renderContext);
}

void MKWidget::UpdateState()
{
  // do nothing
}

void MKWidget::Update()
{
  UpdateState();
}

MKWidget* MKWidget::TestPoint(MKVector3 *pPoint)
{
  // fix up render context
  Matrix attributeMatrix;

  attributeMatrix.SetIdentity();
  attributeMatrix.SetRotationPYR(attributes.rotation);
  attributeMatrix.Scale3x3(attributes.scale);
  attributeMatrix.SetTranslation(attributes.position.x, attributes.position.y, attributes.position.z);

  MKWidgetBoundingVolume client;
  MKWidget_SendMessage(this, MKWM_GETCLIENTVOLUME, (uint32)&client);
  attributeMatrix.Translate(client.position);

  attributeMatrix.Inverse();

  ApplyMatrix(*pPoint, *pPoint, &attributeMatrix);

  if(pPoint->x >= 0.0f && pPoint->y >= 0.0f && pPoint->z >= 0.0f &&
    pPoint->x <= attributes.dimensions.x &&
    pPoint->y <= attributes.dimensions.y &&
    pPoint->z <= attributes.dimensions.z)
    return this;

  return NULL;
}

MKWidget* MKWidget::GetWidgetUnderPoint(MKVector3 point)
{
  if(!(widgetFlags&WF_Visible)) return NULL;

  return TestPoint(&point);
}

void MKWidget::GetLocalToWorldMatrix(Matrix *pMatrix)
{
  pMatrix->SetIdentity();
  pMatrix->SetRotationPYR(attributes.rotation);
  pMatrix->Scale3x3(attributes.scale);
  pMatrix->SetTranslation(attributes.position.x, attributes.position.y, attributes.position.z);

  if(pParent)
  {
    Matrix parent;
    MKWidgetBoundingVolume client;

    pParent->GetLocalToWorldMatrix(&parent);

    MKWidget_SendMessage(pParent, MKWM_GETCLIENTVOLUME, (uint32)&client);
    pMatrix->Translate(client.position);

    pMatrix->Multiply4x4(&parent);
  }
}


// MKWidgetContainer
MKWidgetContainer::MKWidgetContainer()
{
  pDescriptor = &gMKWidgetContainerDescriptor;
  pChildren = NULL;
  childCount = 0;
}

int MKWidgetContainer::MessageHandler(uint32 message, uint32 data)
{
  switch(message)
  {
    case MKWM_ADDCHILD:
    {
      MKWidget *pChild = (MKWidget*)data;

      pChild->pParent = this;
      pChild->pNextSibling = pChildren;
      pChildren = pChild;
      childCount++;

      return 0;
    }

    case MKWM_REMOVECHILD:
    {
      DBGASSERT(false, "Not Done!");
      return 0;
    }

    case MKWM_SENDTOBACK:
    {
      DBGASSERT(false, "Not Done!");
      return 0;
    }

    case MKWM_BRINGTOFRONT:
    {
      DBGASSERT(false, "Not Done!");
      return 0;
    }
  }

  return parent_class::MessageHandler(message, data);
}

void MKWidgetContainer::Draw(MKWidgetRenderContext renderContext)
{
  if(!(widgetFlags&WF_Visible)) return;

  Render(&renderContext);

  for(MKWidget *pWidget = pChildren; pWidget; pWidget = pWidget->pNextSibling)
  {
    pWidget->Draw(renderContext);
  }
}

void MKWidgetContainer::Update()
{
  UpdateState();

  for(MKWidget *pWidget = pChildren; pWidget; pWidget = pWidget->pNextSibling)
  {
    pWidget->Update();
  }
}

MKWidget* MKWidgetContainer::GetWidgetUnderPoint(MKVector3 point)
{
  if(!(widgetFlags&WF_Visible)) return NULL;

  MKWidget *pTest, *pTemp;

  pTest = TestPoint(&point);

  if(!pTest) return NULL;

  for(MKWidget *pWidget = pChildren; pWidget; pWidget = pWidget->pNextSibling)
  {
    pTemp = pWidget->GetWidgetUnderPoint(point);

    if(pTemp) pTest = pTemp;
  }

  return pTest;
}

// MKWidgetSystem
MKWidgetSystem::MKWidgetSystem()
{
  pDescriptor = &gMKWidgetSystemDescriptor;

  pFocus = NULL;
  pMouseFocus = NULL;
  pMouseLock = NULL;
  pClick = NULL;
  pDoubleClick = NULL;
  clickTestButton = 0;
  doubleClickTestButton = 0;

  pName = "MKWidgetSystem";

  memset(keyState, 0, sizeof(keyState));
  memset(keyDownTime, 0, sizeof(keyDownTime));

  oldPointerPos = MKVector3::zero;
}

void MKWidgetSystem::Draw(MKWidgetRenderContext renderContext)
{
  View::GetDefault()->Use();
  View::GetCurrent()->ClearZBuffer();

  parent_class::Draw(renderContext);

  MKVector3 pointer = MKVector3::zero;

  pointer.x = (float)Input_GetButtonState(IDEV_MOUSE, MOUSE_POSX);
  pointer.y = (float)Input_GetButtonState(IDEV_MOUSE, MOUSE_POSY);

  MKPrimitive(PT_TriList|PT_Untextured);
  MKBegin(3);

  MKSetColour(1,1,0,1);
  MKSetPosition(pointer);
  MKSetPosition(pointer+MKVector(20.0f, 10.0f, 0.0f));
  MKSetPosition(pointer+MKVector(10.0f, 20.0f, 0.0f));

  MKEnd();
  MKEndPrimitive();

  gpDebugFont->DrawString2dSimple(pMouseFocus->pDescriptor->pName ? pMouseFocus->pDescriptor->pName : "unnamed", 10, 10);
}

void MKWidgetSystem::UpdateState()
{
  MKVector3 pointer = MKVector3::zero;

  // get pointer pos
  pointer.x = (float)Input_GetButtonState(IDEV_MOUSE, MOUSE_POSX);
  pointer.y = (float)Input_GetButtonState(IDEV_MOUSE, MOUSE_POSY);

  // get widget under pointer
  MKWidget *pMouseOver = GetWidgetUnderPoint(pointer);

  // update mouse focus
  if(!pMouseLock && pMouseFocus != pMouseOver)
  {
    if(pMouseFocus)
      MKWidget_SendMessage(pMouseFocus, MKWM_MOUSEOVER, 0);

    if(pMouseOver)
      MKWidget_SendMessage(pMouseOver, MKWM_MOUSEOVER, 1);

    pMouseFocus = pMouseOver;

    pClick = NULL;
    pDoubleClick = NULL;
    clickTestButton = 0;
    doubleClickTestButton = 0;
  }

  // if mouse moved, send mouse move message
  if(pointer != oldPointerPos)
  {
    MKWidget_SendMessage(pMouseFocus, MKWM_MOUSEMOVE, (uint32)&pointer);

    oldPointerPos = pointer;
  }

  int butonDown = 0;
  int butonUp = 0;

  // check mouse button states
  if(Input_WasButtonPressed(IDEV_MOUSE, MOUSE_LEFT)) butonDown |= 1;
  if(Input_WasButtonPressed(IDEV_MOUSE, MOUSE_RIGHT)) butonDown |= 2;
  if(Input_WasButtonPressed(IDEV_MOUSE, MOUSE_MIDDLE)) butonDown |= 4;
  if(Input_WasButtonPressed(IDEV_MOUSE, MOUSE_EXTRA1)) butonDown |= 8;
  if(Input_WasButtonPressed(IDEV_MOUSE, MOUSE_EXTRA2)) butonDown |= 16;

  if(Input_WasButtonReleased(IDEV_MOUSE, MOUSE_LEFT)) butonUp |= 1;
  if(Input_WasButtonReleased(IDEV_MOUSE, MOUSE_RIGHT)) butonUp |= 2;
  if(Input_WasButtonReleased(IDEV_MOUSE, MOUSE_MIDDLE)) butonUp |= 4;
  if(Input_WasButtonReleased(IDEV_MOUSE, MOUSE_EXTRA1)) butonUp |= 8;
  if(Input_WasButtonReleased(IDEV_MOUSE, MOUSE_EXTRA2)) butonUp |= 16;

  // send button down message
  if(butonDown)
  {
    MKWidget_SendMessage(pMouseFocus, MKWM_MOUSEDOWN, butonDown);
  }

  // send button up messages
  if(butonUp)
  {
    MKWidget_SendMessage(pMouseFocus, MKWM_MOUSEUP, butonUp);
  }

  // update click tests
  clickTestButton |= butonDown;

  // check for click and double click events
  int clicked = clickTestButton & butonUp;
  int doubleClicked = doubleClickTestButton & clicked;
  clicked &= ~doubleClicked;

  // send mouse click message
  if(clicked && pMouseFocus == pMouseOver)
  {
    MKWidget_SendMessage(pMouseFocus, MKWM_MOUSECLICK, clicked);

    doubleClickTestButton |= clicked;
    clickTestButton &= ~clicked;
  }

  // send mouse double click message
  if(doubleClicked && pMouseFocus == pMouseOver)
  {
    MKWidget_SendMessage(pMouseFocus, MKWM_MOUSEDOUBLECLICK, doubleClicked);

    doubleClickTestButton &= ~doubleClicked;
  }
}

MKWidget* MKWidgetSystem::ChangeFocus(MKWidget *pWidget)
{
  MKWidget *pOld = pFocus;
  pFocus = pWidget;
  return pOld;
}

MKWidget* MKWidgetSystem::ChangeMouseFocus(MKWidget *pWidget)
{
  MKWidget *pOld = pMouseLock;
  pMouseLock = pWidget;
  return pOld;
}


// MKWidgetFrame
MKWidgetFrame::MKWidgetFrame()
{
  pDescriptor = &gMKWidgetFrameDescriptor;

  frameStyle = FRM_Raised;
  edgeWidth = 2.0f;
  outerMargin = 0.0f;
  clientMargin = 2.0f;
}

int MKWidgetFrame::MessageHandler(uint32 message, uint32 data)
{
  switch(message)
  {
    case MKWM_GETCLIENTVOLUME:
      ((MKWidgetBoundingVolume*)data)->position = MKVector(clientMargin, clientMargin, 0.0f);
      ((MKWidgetBoundingVolume*)data)->dimensions = MKVector(attributes.dimensions.x-clientMargin*2, attributes.dimensions.y-clientMargin*2, attributes.dimensions.z);
      return 0;
  }

  return parent_class::MessageHandler(message, data);
}

void MKWidgetFrame::Render(MKWidgetRenderContext *pRenderContext)
{
  parent_class::Render(pRenderContext);

  MKPrimitive(PT_TriStrip);
  MKSetBaseColour(pRenderContext->colour);
  MKSetMatrix(pRenderContext->tramsform);

  MKVector3 clientVector = MKVector(clientMargin, clientMargin, 0.0f);

  if((frameStyle & 0x7) == 0)
  {
    WidgetDrawSquare(-clientVector, attributes.dimensions - clientVector, edgeWidth, MKWidgetWindow::windowColour);
  }
  else
  {
    uint32 style = (frameStyle - 1) & 0x3;
    MKVector3 margin = MKVector(outerMargin, outerMargin, 0.0f);

    WidgetDrawBevel(margin - clientVector, attributes.dimensions - margin - clientVector, edgeWidth, MKVector4::one, MKVector4::identity, (style&1) != 0, (frameStyle&2) != 0);
  }

  MKEndPrimitive();
}


// MKWidgetWindow
MKVector4 MKWidgetWindow::windowColour = MKVector(0.7f, 0.7f, 0.7f, 1.0f);
MKVector4 MKWidgetWindow::clientColour = MKVector(0.4f, 0.4f, 0.4f, 1.0f);
MKVector4 MKWidgetWindow::titleColour = MKVector(0.3f, 0.6f, 1.0f, 1.0f);
float MKWidgetWindow::borderWidth = 2.0f;
float MKWidgetWindow::titleHeight = 20.0f;

MKWidgetWindow::MKWidgetWindow()
{
  pDescriptor = &gMKWidgetWindowDescriptor;

  widgetFlags = WF_Enabled;
  windowFlags = WF_Border|WF_Title|WF_CloseButton|WF_SysMenu;

  edgeWidth = borderWidth;
  clientMargin = borderWidth;

  scrollOffset = MKVector3::zero;

  titleBar.pParent = this;
  MKWidget_Move(&titleBar, MKVector(0.0f, -titleHeight, 0.0f));
}

int MKWidgetWindow::MessageHandler(uint32 message, uint32 data)
{
  switch(message)
  {
    case MKWM_GETCLIENTVOLUME:
    {
      if(windowFlags&WF_Title)
      {
        MKWidgetFrame::MessageHandler(MKWM_GETCLIENTVOLUME, data);

        ((MKWidgetBoundingVolume*)data)->position.y += titleHeight;
        ((MKWidgetBoundingVolume*)data)->dimensions.y -= titleHeight;
      }

      return 0;
    }

    case MKWM_SIZE:
    {
      MKVector3 *pSize = (MKVector3*)data;

      if(pSize->x < 100.0f) pSize->x = 100.0f;
      if(pSize->y < 75.0f) pSize->y = 75.0f;

      MKWidget_Resize(&titleBar, MKVector(pSize->x - borderWidth*2, titleHeight, 0.0f));

      break;
    }

    case MKWM_SETNAME:
      MKWidget_SendMessage(&titleBar, MKWM_SETNAME, data);
      break;

    case WWM_CLOSE:
      MKWidget_Show(this, 0);
      break;
  }

  return parent_class::MessageHandler(message, data);
}

void MKWidgetWindow::Render(MKWidgetRenderContext *pRenderContext)
{
  pRenderContext->tramsform.Translate(MKVector(0.0f, -titleHeight, 0.0f));
  parent_class::Render(pRenderContext);
  pRenderContext->tramsform.Translate(MKVector(0.0f, titleHeight, 0.0f));

  // if we have a titlebar, render it and offset client
  if(windowFlags & WF_Title)
  {
    titleBar.Draw(*pRenderContext);
  }

  // render client region
  MKWidgetBoundingVolume clientBounds;
  MessageHandler(MKWM_GETCLIENTVOLUME, (uint32)&clientBounds);

  MKPrimitive(PT_TriStrip|PT_Untextured);
  MKSetBaseColour(pRenderContext->colour);
  MKSetMatrix(pRenderContext->tramsform);

  WidgetDrawBox(MKVector3::zero, clientBounds.dimensions, windowColour);

  MKEndPrimitive();
}

MKWidget* MKWidgetWindow::TestPoint(MKVector3 *pPoint)
{
  MKWidget *pWidget = parent_class::TestPoint(pPoint);

  MKWidget *pTemp = titleBar.GetWidgetUnderPoint(*pPoint);
  if(pTemp) pWidget = pTemp;

  return pWidget;
}


// MKWidgetWindowTitleBar
MKWidgetWindowTitleBar::MKWidgetWindowTitleBar()
{
  pDescriptor = &gMKWidgetWindowTitleBarDescriptor;

  titleFlags = WF_ShowCaption|WF_CloseButton|WF_SysMenu;

  title.pParent = this;
  close.pParent = this;

  dragging = false;

  MKVector3 temp = MKVector(2.0f, 2.0f, 0.0f);
  MKWidget_Move(&title, temp);
  MKWidget_SendMessage(&title, MKWidgetString::WSM_SETJUSTIFY, FONTJUSTIFY_CENTRE_LEFT);

  MKWidget_SetName(&close, "X");
}

int MKWidgetWindowTitleBar::MessageHandler(uint32 message, uint32 data)
{
  switch(message)
  {
    case MKWM_SIZE:
    {
      MKVector3 *pSize = (MKVector3*)data;
      MKWidget_Resize(&title, MKVector(pSize->x - 6.0f - pSize->y, pSize->y - 4.0f, 0.0f));

      MKWidget_Move(&close, MKVector(pSize->x - pSize->y - 2.0f, 2.0f, 0.0f));
      MKWidget_Resize(&close, MKVector(pSize->y, pSize->y - 4.0f, 0.0f));
      break;
    }

    case MKWM_SETNAME:
      MKWidget_SendMessage(&title, MKWidgetString::WSM_SETTEXT, data);
      break;

    case MKWM_NOTIFY:
    {
      if((MKWidget*)data == &close)
      {
        MKWidget_SendMessage(pParent, MKWidgetWindow::WWM_CLOSE, 0);
      }
      return 0;
    }

    case MKWM_MOUSEDOWN:
      if(data & 1)
      {
        MKWidget_GetLocalMousePos(this, &mousePos);
        MKWidget_SetMouseFocus(this);
        dragging = true;
      }
      break;

    case MKWM_MOUSEUP:
      if(data & 1)
      {
        MKWidget_ReleaseMouseFocus();
        dragging = false;
      }
      break;

    case MKWM_MOUSEMOVE:
      if(dragging)
      {
        MKVector3 newPos, change;
        MKWidget_GetLocalMousePos(this, &newPos);

        change = newPos - mousePos;

        MKWidget_SendMessage(pParent, MKWM_GETPOS, (uint32)&newPos);
        newPos += change;

        MKWidget_SendMessage(pParent, MKWM_MOVE, (uint32)&newPos);
      }
      if(sizing)
      {

      }
      if(scaling)
      {

      }
      if(rotating)
      {

      }
      break;

    case MKWM_SETFOCUS:
      return 0;
  }

  return parent_class::MessageHandler(message, data);
}

void MKWidgetWindowTitleBar::Render(MKWidgetRenderContext *pRenderContext)
{
  parent_class::Render(pRenderContext);

  MKPrimitive(PT_TriStrip);
  MKSetBaseColour(pRenderContext->colour);
  MKSetMatrix(pRenderContext->tramsform);

  WidgetDrawBox(MKVector3::zero, attributes.dimensions, MKWidgetWindow::titleColour);

  MKEndPrimitive();

  title.Draw(*pRenderContext);
  close.Draw(*pRenderContext);
}

MKWidget* MKWidgetWindowTitleBar::TestPoint(MKVector3 *pPoint)
{
  MKWidget *pWidget = parent_class::TestPoint(pPoint);

  if(pWidget)
  {
    MKWidget *pTemp;
    
    pTemp = title.GetWidgetUnderPoint(*pPoint);
    if(pTemp) pWidget = pTemp;
    pTemp = close.GetWidgetUnderPoint(*pPoint);
    if(pTemp) pWidget = pTemp;
  }

  return pWidget;
}


// MKWidgetString
MKWidgetString::MKWidgetString()
{
  pDescriptor = &gMKWidgetStringDescriptor;
  pFont = gpDebugFont;
  pText = NULL;
  fontJustify = FONTJUSTIFY_TOP_LEFT;
}

int MKWidgetString::MessageHandler(uint32 message, uint32 data)
{
  switch(message)
  {
    case WSM_SETTEXT:
      pText = (char *)data;
      break;

    case WSM_GETTEXT:
      *(char **)data = pText;
      break;

    case WSM_SETJUSTIFY:
      fontJustify = data;
      break;

    ////////////////////////////////////////////////////////////////////////
    // not sure if all these do need to be redirected to the parent since
    // this widget can NEVER be the focus widget anyway
    ////////////////////////////////////////////////////////////////////////
    case MKWM_GAMEPAD:
    case MKWM_KEY:
    case MKWM_KEYDOWN:
    case MKWM_KEYUP:
    case MKWM_MOUSEDOWN:
    case MKWM_MOUSEUP:
    case MKWM_MOUSECLICK:
    case MKWM_MOUSEDOUBLECLICK:
    case MKWM_MOUSEWHEEL:
    case MKWM_DRAGSTART:
    case MKWM_DRAGOVER:
    case MKWM_DROP:
    ////////////////////////////////////////////////////////////////////////

    case MKWM_MOUSEMOVE:
    case MKWM_MOUSEOVER:
    case MKWM_SETFOCUS:
    case MKWM_SETMOUSEFOCUS:
      return MKWidget_SendMessage(pParent, message, data);
  }

  return parent_class::MessageHandler(message, data);
}

void MKWidgetString::Render(MKWidgetRenderContext *pRenderContext)
{
  parent_class::Render(pRenderContext);

  if(pText)
  {
    MKVector3 offset = attributes.dimensions * 0.5f;
    offset = ApplyRotMatrix(offset, &pRenderContext->tramsform);

    Matrix mat = pRenderContext->tramsform;
    mat.Translate(offset);

    pFont->DrawString(pText, attributes.dimensions.x, attributes.dimensions.y, &mat, fontJustify, 0x80808080);
  }
//  pFont->pText, 0.0f, 0.0f, 0x80808080, pFont->GetScale(attributes.dimensions.y));
}

// MKWidgetButton
MKWidgetButton::MKWidgetButton()
{
  pDescriptor = &gMKWidgetButtonDescriptor;
  buttonPressed = false;
  buttonDown = false;

  label.pParent = this;
  MKWidget_SendMessage(&label, MKWidgetString::WSM_SETJUSTIFY, FONTJUSTIFY_CENTRE);
}

int MKWidgetButton::MessageHandler(uint32 message, uint32 data)
{
  switch(message)
  {
    case MKWM_MOVE:
      {
        MKVector3 temp = MKVector(3.0f, 3.0f, 0.0f);
        MKWidget_Move(&label, temp);
      }
      break;

    case MKWM_SIZE:
      {
        MKVector3 newSize = *(MKVector3*)data - MKVector(6.0f, 6.0f, 0.0f);
        MKWidget_Resize(&label, newSize);
      }
      break;

    case MKWM_MOUSEDOWN:
      MKWidget_SetMouseFocus(this);
      buttonPressed = true;
      buttonDown = true;
      break;

    case MKWM_MOUSEUP:
      MKWidget_ReleaseMouseFocus();
      buttonPressed = false;
      buttonDown = false;
      break;

    case MKWM_MOUSEMOVE:
      if(buttonPressed)
      {
        MKVector3 pos;
        MKWidget_GetLocalMousePos(this, &pos);

        if(pos.x < 0.0f || pos.y < 0.0f || pos.x > attributes.dimensions.x || pos.y > attributes.dimensions.y)
          buttonDown = false;
        else
          buttonDown = true;
      }
      break;

    case MKWM_MOUSECLICK:
    case MKWM_MOUSEDOUBLECLICK:
      {
        if(pParent)
          MKWidget_SendMessage(pParent, MKWM_NOTIFY, (uint32)this);
      }
      break;

    case MKWM_SETNAME:
      MKWidget_SendMessage(&label, MKWidgetString::WSM_SETTEXT, data);
      break;
  }

  return parent_class::MessageHandler(message, data);
}

void MKWidgetButton::Render(MKWidgetRenderContext *pRenderContext)
{
  parent_class::Render(pRenderContext);

  MKPrimitive(PT_TriStrip|PT_Untextured);
  MKSetBaseColour(pRenderContext->colour);
  MKSetMatrix(pRenderContext->tramsform);

  WidgetDrawBevel(MKVector3::zero, attributes.dimensions, 1.0f, MKVector4::one, MKVector4::identity, buttonDown);
  WidgetDrawBox(MKVector(1.0f, 1.0f, 0.0f), attributes.dimensions-MKVector(1.0f, 1.0f, 0.0f), MKWidgetWindow::windowColour);

  MKEndPrimitive();

  label.Draw(*pRenderContext);
}


MKWidget* MKWidgetButton::TestPoint(MKVector3 *pPoint)
{
  MKWidget *pWidget = parent_class::TestPoint(pPoint);

  if(pWidget)
  {
    MKWidget *pTemp;
    
    pTemp = label.GetWidgetUnderPoint(*pPoint);
    if(pTemp) pWidget = pTemp;
  }

  return pWidget;
}


// MKWidgetCheckMark
MKWidgetCheckMark::MKWidgetCheckMark()
{
  pDescriptor = &gMKWidgetCheckMarkDescriptor;

  checkState = 0;
  buttonPressed = false;
  buttonDown = false;
}

int MKWidgetCheckMark::MessageHandler(uint32 message, uint32 data)
{
  switch(message)
  {
    case MKWM_MOUSEDOWN:
      MKWidget_SetMouseFocus(this);
      buttonPressed = true;
      buttonDown = true;
      break;

    case MKWM_MOUSEUP:
      MKWidget_ReleaseMouseFocus();
      buttonPressed = false;
      buttonDown = false;
      break;

    case MKWM_MOUSEMOVE:
      if(buttonPressed)
      {
        MKVector3 pos;
        MKWidget_GetLocalMousePos(this, &pos);

        if(pos.x < 0.0f || pos.y < 0.0f || pos.x > attributes.dimensions.x || pos.y > attributes.dimensions.y)
          buttonDown = false;
        else
          buttonDown = true;
      }
      break;

    case MKWM_MOUSECLICK:
    case MKWM_MOUSEDOUBLECLICK:
      {
        if(pParent)
          MKWidget_SendMessage(pParent, MKWM_NOTIFY, (uint32)this);

        checkState = !checkState;
      }
      break;
  }

  return parent_class::MessageHandler(message, data);
}

void MKWidgetCheckMark::Render(MKWidgetRenderContext *pRenderContext)
{
  parent_class::Render(pRenderContext);

  MKPrimitive(PT_TriStrip|PT_Untextured);
  MKSetBaseColour(pRenderContext->colour);
  MKSetMatrix(pRenderContext->tramsform);

  WidgetDrawBevel(MKVector3::zero, attributes.dimensions, 1.0f, MKVector4::one, MKVector4::identity, true);
  WidgetDrawBox(MKVector(1.0f, 1.0f, 0.0f), attributes.dimensions-MKVector(1.0f, 1.0f, 0.0f), (buttonDown || (checkState==2)) ? MKWidgetWindow::windowColour : MKVector4::one);

  if(checkState)
  {
    MKBegin(4);
    MKSetColour(MKVector4::identity);
    MKSetPosition(2.0f, attributes.dimensions.y*0.5f, 0.0f);
    MKSetPosition(attributes.dimensions.x*0.4f, attributes.dimensions.y-2.0f, 0.0f);
    MKSetPosition(attributes.dimensions.x*0.4f, (attributes.dimensions.y-2.0f)*0.7f, 0.0f);
    MKSetPosition(attributes.dimensions.x-2.0f, 2.0f, 0.0f);
    MKEnd();
  }

  MKEndPrimitive();
}


// MKWidgetCheckBox
MKWidgetCheckBox::MKWidgetCheckBox()
{
  pDescriptor = &gMKWidgetCheckBoxDescriptor;

  label.pParent = this;
  check.pParent = this;

  MKWidget_Move(&check, MKVector(2.0f, 2.0f, 0.0f));

  MKWidget_SendMessage(&label, MKWidgetString::WSM_SETJUSTIFY, FONTJUSTIFY_CENTRE_LEFT);
}

int MKWidgetCheckBox::MessageHandler(uint32 message, uint32 data)
{
  switch(message)
  {
    case MKWM_SETNAME:
      MKWidget_SendMessage(&label, MKWidgetString::WSM_SETTEXT, data);
      break;

    case MKWM_SIZE:
      {
        MKVector3 *pSize = (MKVector3*)data;
        MKWidget_Resize(&check, MKVector(pSize->y-4.0f, pSize->y-4.0f, 0.0f));

        MKWidget_Move(&label, MKVector(pSize->y, 0.0f, 0.0f));
        MKWidget_Resize(&label, MKVector(pSize->x - pSize->y, pSize->y, 0.0f));
      }
      break;
  }

  return parent_class::MessageHandler(message, data);
}

void MKWidgetCheckBox::Render(MKWidgetRenderContext *pRenderContext)
{
  parent_class::Render(pRenderContext);

  check.Draw(*pRenderContext);
  label.Draw(*pRenderContext);
}

MKWidget* MKWidgetCheckBox::TestPoint(MKVector3 *pPoint)
{
  MKWidget *pWidget = parent_class::TestPoint(pPoint);

  if(pWidget)
  {
    MKWidget *pTemp;
    
    pTemp = label.GetWidgetUnderPoint(*pPoint);
    if(pTemp) pWidget = pTemp;
    pTemp = check.GetWidgetUnderPoint(*pPoint);
    if(pTemp) pWidget = pTemp;
  }

  return pWidget;
}


// MKWidgetThumb
MKWidgetThumb::MKWidgetThumb()
{
  pDescriptor = &gMKWidgetThumbDescriptor;

  minScroll = MKVector3::zero;
  maxScroll = MKVector3::zero;
  dragging = false;
}

int MKWidgetThumb::MessageHandler(uint32 message, uint32 data)
{
  switch(message)
  {
    case WTM_SETMINPOS:
      {
        MKVector3 pos;

        minScroll = *(MKVector3*)data;

        pos = attributes.position + attributes.dimensions*0.5f;
        pos = Min(Max(pos, minScroll), maxScroll);
        pos -= attributes.dimensions*0.5f;

        MKWidget_Move(this, pos);
      }
      break;

    case WTM_SETMAXPOS:
      {
        MKVector3 pos;

        maxScroll = *(MKVector3*)data;

        pos = attributes.position + attributes.dimensions*0.5f;
        pos = Min(Max(pos, minScroll), maxScroll);
        pos -= attributes.dimensions*0.5f;

        MKWidget_Move(this, pos);
      }
      break;

    case MKWM_MOUSEDOWN:
      if(data & 1)
      {
        MKWidget_GetLocalMousePos(this, &mousePos);
        MKWidget_SetMouseFocus(this);
        dragging = true;
      }
      break;

    case MKWM_MOUSEUP:
      if(data & 1)
      {
        MKWidget_ReleaseMouseFocus();
        dragging = false;
      }
      break;

    case MKWM_MOUSEMOVE:
      if(dragging)
      {
        MKVector3 newPos, change;
        MKWidget_GetLocalMousePos(this, &newPos);

        change = newPos - mousePos;
        newPos = attributes.position + change;

        newPos += attributes.dimensions*0.5f;
        newPos = Min(Max(newPos, minScroll), maxScroll);
        newPos -= attributes.dimensions*0.5f;

        if(newPos != attributes.position)
        {
          MKWidget_SendMessage(this, MKWM_MOVE, (uint32)&newPos);
          MKWidget_SendMessage(pParent, MKWM_NOTIFY, (uint32)this);
        }
      }
      break;
  }

  return parent_class::MessageHandler(message, data);
}

void MKWidgetThumb::Render(MKWidgetRenderContext *pRenderContext)
{
  parent_class::Render(pRenderContext);

  MKPrimitive(PT_TriStrip|PT_Untextured);
  MKSetBaseColour(pRenderContext->colour);
  MKSetMatrix(pRenderContext->tramsform);

  WidgetDrawBox(MKVector3::zero, attributes.dimensions, MKWidgetWindow::windowColour);
  WidgetDrawBevel(MKVector3::zero, attributes.dimensions, 1.0f, MKVector4::one, MKVector4::identity);

  MKEndPrimitive();
}


// MKWidgetScrollBar
MKWidgetScrollBar::MKWidgetScrollBar()
{
  pDescriptor = &gMKWidgetScrollBarDescriptor;

  increase.pParent = this;
  decrease.pParent = this;
  thumb.pParent = this;

  value = 0.0f;
  minVal = 0.0f;
  maxVal = 1.0f;
  inc = 0.1f;
}

int MKWidgetScrollBar::MessageHandler(uint32 message, uint32 data)
{
  switch(message)
  {
    case MKWM_SIZE:
      {
        MKVector3 *pDim = (MKVector3*)data;
        MKVector3 temp;
        bool vertical = pDim->y > pDim->x ? true : false;

        if(vertical)
        {
          pDim->y = Max(pDim->y, pDim->x*2.0f + 4.0f);
          temp = MKVector(pDim->x, pDim->x, 0.0f);
          thumbWidth = Min(pDim->y - pDim->x*2.0f, pDim->x);

          MKWidget_Resize(&decrease, temp);
          MKWidget_Resize(&increase, temp);
          MKWidget_Move(&increase, MKVector(0.0f, pDim->y - pDim->x, 0.0f));
          MKWidget_Resize(&thumb, MKVector(pDim->x, thumbWidth, 0.0f));

          temp = MKVector(pDim->x*0.5f, pDim->x + thumbWidth*0.5f, 0.0f);
          MKWidget_SendMessage(&thumb, MKWidgetThumb::WTM_SETMINPOS, (uint32)&temp);
          temp = MKVector(pDim->x*0.5f, pDim->y - pDim->x - thumbWidth*0.5f, 0.0f);
          MKWidget_SendMessage(&thumb, MKWidgetThumb::WTM_SETMAXPOS, (uint32)&temp);
        }
        else
        {
          pDim->x = Max(pDim->x, pDim->y*2.0f + 4.0f);
          temp = MKVector(pDim->y, pDim->y, 0.0f);
          thumbWidth = Min(pDim->x - pDim->y*2.0f, pDim->y);

          MKWidget_Resize(&decrease, temp);
          MKWidget_Resize(&increase, temp);
          MKWidget_Move(&increase, MKVector(pDim->x - pDim->y, 0.0f, 0.0f));
          MKWidget_Resize(&thumb, MKVector(thumbWidth, pDim->y, 0.0f));

          temp = MKVector(pDim->y + thumbWidth*0.5f, pDim->y*0.5f, 0.0f);
          MKWidget_SendMessage(&thumb, MKWidgetThumb::WTM_SETMINPOS, (uint32)&temp);
          temp = MKVector(pDim->x - pDim->y - thumbWidth*0.5f, pDim->y*0.5f, 0.0f);
          MKWidget_SendMessage(&thumb, MKWidgetThumb::WTM_SETMAXPOS, (uint32)&temp);
        }
      }
      break;

    case WSM_SETVALUE:
      value = *(float*)data;
      break;

    case WSM_GETVALUE:
      *(float*)data = value;
      break;

    case WSM_SETMIN:
      minVal = *(float*)data;
      break;

    case WSM_SETMAX:
      maxVal = *(float*)data;
      break;

    case WSM_SETINCREMENT:
      inc = *(float*)data;
      break;
  }

  return parent_class::MessageHandler(message, data);
}

void MKWidgetScrollBar::Render(MKWidgetRenderContext *pRenderContext)
{
  parent_class::Render(pRenderContext);

  MKPrimitive(PT_TriStrip|PT_Untextured);
  MKSetBaseColour(pRenderContext->colour);
  MKSetMatrix(pRenderContext->tramsform);

  WidgetDrawBox(MKVector3::zero, attributes.dimensions, MKVector(0,0,1,1));

  MKEndPrimitive();

  increase.Draw(*pRenderContext);
  decrease.Draw(*pRenderContext);
  thumb.Draw(*pRenderContext);
}

MKWidget* MKWidgetScrollBar::TestPoint(MKVector3 *pPoint)
{
  MKWidget *pWidget = parent_class::TestPoint(pPoint);

  if(pWidget)
  {
    MKWidget *pTemp;
    
    pTemp = increase.GetWidgetUnderPoint(*pPoint);
    if(pTemp) pWidget = pTemp;
    pTemp = decrease.GetWidgetUnderPoint(*pPoint);
    if(pTemp) pWidget = pTemp;
    pTemp = thumb.GetWidgetUnderPoint(*pPoint);
    if(pTemp) pWidget = pTemp;
  }

  return pWidget;
}


// MKWidgetSlider
MKWidgetSlider::MKWidgetSlider()
{
  pDescriptor = &gMKWidgetSliderDescriptor;

  thumb.pParent = this;

  value = 0.0f;
  minVal = 0.0f;
  maxVal = 1.0f;
  inc = 0.1f;
}

int MKWidgetSlider::MessageHandler(uint32 message, uint32 data)
{
  switch(message)
  {
    case MKWM_SIZE:
      {
        MKVector3 *pDim = (MKVector3*)data;
        MKVector3 temp;
        bool vertical = pDim->y > pDim->x ? true : false;

        if(vertical)
        {
          pDim->y = Max(pDim->y, pDim->x*2.0f + 4.0f);
          thumbWidth = pDim->x*0.5f;

          MKWidget_Resize(&thumb, MKVector(pDim->x, thumbWidth, 0.0f));

          temp = MKVector(pDim->x*0.5f, thumbWidth*0.5f, 0.0f);
          MKWidget_SendMessage(&thumb, MKWidgetThumb::WTM_SETMINPOS, (uint32)&temp);
          temp = MKVector(pDim->x*0.5f, pDim->y - thumbWidth*0.5f, 0.0f);
          MKWidget_SendMessage(&thumb, MKWidgetThumb::WTM_SETMAXPOS, (uint32)&temp);
        }
        else
        {
          pDim->x = Max(pDim->x, pDim->y*2.0f + 4.0f);
          thumbWidth = pDim->y*0.5f;

          MKWidget_Resize(&thumb, MKVector(thumbWidth, pDim->y, 0.0f));

          temp = MKVector(thumbWidth*0.5f, pDim->y*0.5f, 0.0f);
          MKWidget_SendMessage(&thumb, MKWidgetThumb::WTM_SETMINPOS, (uint32)&temp);
          temp = MKVector(pDim->x - thumbWidth*0.5f, pDim->y*0.5f, 0.0f);
          MKWidget_SendMessage(&thumb, MKWidgetThumb::WTM_SETMAXPOS, (uint32)&temp);
        }
      }
      break;

    case WSM_SETVALUE:
      value = *(float*)data;
      break;

    case WSM_GETVALUE:
      *(float*)data = value;
      break;

    case WSM_SETMIN:
      minVal = *(float*)data;
      break;

    case WSM_SETMAX:
      maxVal = *(float*)data;
      break;

    case WSM_SETINCREMENT:
      inc = *(float*)data;
      break;
  }

  return parent_class::MessageHandler(message, data);
}

void MKWidgetSlider::Render(MKWidgetRenderContext *pRenderContext)
{
  parent_class::Render(pRenderContext);

  MKPrimitive(PT_TriStrip|PT_Untextured);
  MKSetBaseColour(pRenderContext->colour);
  MKSetMatrix(pRenderContext->tramsform);

  bool vertical = attributes.dimensions.x < attributes.dimensions.y ? true : false;

  if(vertical)
    WidgetDrawBevel(
      MKVector(attributes.dimensions.x*0.5f - 2.0f, attributes.dimensions.x*0.25f - 2.0f, 0.0f),
      MKVector(attributes.dimensions.x*0.5f + 2.0f, attributes.dimensions.y - attributes.dimensions.x*0.25f + 2.0f, 0.0f),
      2.0f, MKVector4::one, MKVector4::identity, true);
  else
    WidgetDrawBevel(
      MKVector(attributes.dimensions.y*0.25f - 2.0f, attributes.dimensions.y*0.5f - 2.0f, 0.0f),
      MKVector(attributes.dimensions.x - attributes.dimensions.y*0.25f + 2.0f, attributes.dimensions.y*0.5f + 2.0f, 0.0f),
      2.0f, MKVector4::one, MKVector4::identity, true);

  MKEndPrimitive();

  thumb.Draw(*pRenderContext);
}

MKWidget* MKWidgetSlider::TestPoint(MKVector3 *pPoint)
{
  MKWidget *pWidget = parent_class::TestPoint(pPoint);

  if(pWidget)
  {
    MKWidget *pTemp;
    
    pTemp = thumb.GetWidgetUnderPoint(*pPoint);
    if(pTemp) pWidget = pTemp;
  }

  return pWidget;
}

