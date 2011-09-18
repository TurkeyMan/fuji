#include "Haku.h"
#include "UI/HKWidget.h"
#include "UI/HKUI.h"

#include "MFTypes.h"
#include "MFCollision.h"

const EnumKeypair HKWidget::sJustifyKeys[] =
{
	{ "TopLeft", TopLeft },
	{ "TopCenter", TopCenter },
	{ "TopRight", TopRight },
	{ "TopFill", TopFill },
	{ "CenterLeft", CenterLeft },
	{ "Center", Center },
	{ "CenterRight", CenterRight },
	{ "CenterFill", CenterFill },
	{ "BottomLeft", BottomLeft },
	{ "BottomCenter", BottomCenter },
	{ "BottomRight", BottomRight },
	{ "BottomFill", BottomFill },
	{ "FillLeft", FillLeft },
	{ "FillCenter", FillCenter },
	{ "FillRight", FillRight },
	{ "Fill", Fill },
	{ "None", None }
};

HKWidget::HKWidget()
{
	pTypeName = "HKWidget";

	pos = MFVector::zero;
	size.Set(60, 40, 0);
	colour = MFVector::white;
	scale = MFVector::one;
	rot = MFVector::zero;

	layoutMargin = MFVector::zero;
	layoutJustification = None;
	layoutWeight = 1.f;

	pRenderer = NULL;
	pParent = NULL;

	zDepth = 0;

	bVisible = true;
	bEnabled = true;
	bParentEnabled = true;

	bMatrixDirty = bInvMatrixDirty = true;
}

HKWidget::~HKWidget()
{
	if(pRenderer)
	{
		delete pRenderer;
		pRenderer = NULL;
	}
}

HKWidget *HKWidget::Create()
{
	return new HKWidget();
}

void HKWidget::Update()
{
	// update the children
	int numChildren = GetNumChildren();
	for(int a=0; a<numChildren; ++a)
	{
		HKWidget *pChild = GetChild(a);
		pChild->Update();
	}
}

const MFMatrix &HKWidget::GetTransform()
{
	if(bMatrixDirty)
	{
		if(rot == MFVector::zero)
		{
			matrix.SetScale(scale);
		}
		else
		{
			// build the axiis from the rotation vector
			matrix.SetRotationYPR(rot.y, rot.x, rot.z);

			// scale the axiis
			matrix.SetXAxis3(matrix.GetXAxis() * scale.x);
			matrix.SetYAxis3(matrix.GetYAxis() * scale.y);
			matrix.SetZAxis3(matrix.GetZAxis() * scale.z);
		}

		// and set the position
		matrix.SetTrans3(pos);

		// and multiply in the parent
		if(pParent)
			matrix.Multiply(pParent->GetTransform(), matrix);

		bMatrixDirty = false;
	}

	return matrix;
}

const MFMatrix &HKWidget::GetInvTransform()
{
	if(bInvMatrixDirty)
	{
		invMatrix.Inverse(GetTransform());
		bInvMatrixDirty = false;
	}
	return invMatrix;
}

void HKWidget::SetRenderer(HKWidgetRenderer *pRenderer)
{
	this->pRenderer = pRenderer;
}

HKUserInterface &HKWidget::GetUI()
{
	return HKUserInterface::Get();
}

int HKWidget::GetNumChildren() const
{
	return 0;
}

HKWidget *HKWidget::GetChild(int index) const
{
	return NULL;
}

void HKWidget::SetPropertyB(const char *pProperty, bool bValue)
{
	if(!MFString_CaseCmp(pProperty, "enabled"))
		SetEnabled(bValue);
	else if(!MFString_CaseCmp(pProperty, "visible"))
		SetVisible(bValue);
	else if(pRenderer)
		pRenderer->SetPropertyB(pProperty, bValue);
}

void HKWidget::SetPropertyI(const char *pProperty, int value)
{
	if(!MFString_CaseCmp(pProperty, "layout_zDepth"))
		zDepth = value;
	else if(pRenderer)
		pRenderer->SetPropertyI(pProperty, value);
}

void HKWidget::SetPropertyF(const char *pProperty, float value)
{
	if(!MFString_CaseCmp(pProperty, "layout_weight"))
		SetLayoutWeight(value);
	if(pRenderer)
		pRenderer->SetPropertyF(pProperty, value);
}

void HKWidget::SetPropertyV(const char *pProperty, const MFVector& value)
{
	if(!MFString_CaseCmp(pProperty, "position"))
		SetPosition(value);
	else if(!MFString_CaseCmp(pProperty, "size"))
		SetSize(value);
	else if(!MFString_CaseCmp(pProperty, "scale"))
		SetScale(value);
	else if(!MFString_CaseCmp(pProperty, "colour"))
		SetColour(value);
	else if(!MFString_CaseCmp(pProperty, "rotation"))
		SetRotation(value);
	else if(!MFString_CaseCmp(pProperty, "layout_margin"))
		SetLayoutMargin(value);
	else if(pRenderer)
		pRenderer->SetPropertyV(pProperty, value);
}

void HKWidget::SetPropertyS(const char *pProperty, const char *pValue)
{
	if(!MFString_CaseCmp(pProperty, "enabled"))
		SetEnabled(HKWidget_GetBoolFromString(pValue));
	else if(!MFString_CaseCmp(pProperty, "visible"))
		SetVisible(HKWidget_GetBoolFromString(pValue));
	else if(!MFString_CaseCmp(pProperty, "layout_zDepth"))
		zDepth = MFString_AsciiToInteger(pValue);
	else if(!MFString_CaseCmp(pProperty, "layout_weight"))
		SetLayoutWeight(MFString_AsciiToFloat(pValue));
	else if(!MFString_CaseCmp(pProperty, "position"))
		SetPosition(HKWidget_GetVectorFromString(pValue));
	else if(!MFString_CaseCmp(pProperty, "size"))
		SetSize(HKWidget_GetVectorFromString(pValue));
	else if(!MFString_CaseCmp(pProperty, "scale"))
		SetScale(HKWidget_GetVectorFromString(pValue));
	else if(!MFString_CaseCmp(pProperty, "colour"))
		SetColour(HKWidget_GetVectorFromString(pValue));
	else if(!MFString_CaseCmp(pProperty, "rotation"))
		SetRotation(HKWidget_GetVectorFromString(pValue));
	else if(!MFString_CaseCmp(pProperty, "layout_margin"))
		SetLayoutMargin(HKWidget_GetVectorFromString(pValue));
	else if(!MFString_CaseCmp(pProperty, "width"))
	{
		MFVector size = GetSize();
		size.x = MFString_AsciiToFloat(pValue);
		SetSize(size);
	}
	else if(!MFString_CaseCmp(pProperty, "height"))
	{
		MFVector size = GetSize();
		size.y = MFString_AsciiToFloat(pValue);
		SetSize(size);
	}
	else if(!MFString_CaseCmp(pProperty, "name"))
		name = pValue;
	else if(!MFString_CaseCmp(pProperty, "layout_justification"))
		SetLayoutJustification((Justification)HKWidget_GetEnumValue(pValue, sJustifyKeys));
	else if(pRenderer)
		pRenderer->SetPropertyS(pProperty, pValue);
}

bool HKWidget::GetPropertyB(const char *pProperty)
{
	if(!MFString_CaseCmp(pProperty, "enabled"))
		return GetEnabled();
	else if(!MFString_CaseCmp(pProperty, "visible"))
		return GetVisible();
	else if(pRenderer)
		return pRenderer->GetPropertyB(pProperty);
	return false;
}

int HKWidget::GetPropertyI(const char *pProperty)
{
	if(!MFString_CaseCmp(pProperty, "layout_zDepth"))
		return zDepth;
	else if(pRenderer)
		return pRenderer->GetPropertyI(pProperty);
	return 0;
}

float HKWidget::GetPropertyF(const char *pProperty)
{
	if(!MFString_CaseCmp(pProperty, "layout_weight"))
		return GetLayoutWeight();
	else if(pRenderer)
		pRenderer->GetPropertyF(pProperty);
	return 0.f;
}

const MFVector &HKWidget::GetPropertyV(const char *pProperty)
{
	if(!MFString_CaseCmp(pProperty, "position"))
		return GetPosition();
	else if(!MFString_CaseCmp(pProperty, "size"))
		return GetSize();
	else if(!MFString_CaseCmp(pProperty, "scale"))
		return GetScale();
	else if(!MFString_CaseCmp(pProperty, "colour"))
		return GetColour();
	else if(!MFString_CaseCmp(pProperty, "rotation"))
		return GetRotation();
	else if(!MFString_CaseCmp(pProperty, "layout_margin"))
		return GetLayoutMargin();
	else if(pRenderer)
		return pRenderer->GetPropertyV(pProperty);
	return MFVector::zero;
}

MFString HKWidget::GetPropertyS(const char *pProperty)
{
	if(!MFString_CaseCmp(pProperty, "name"))
		return name;
	else if(!MFString_CaseCmp(pProperty, "layout_justification"))
		return HKWidget_GetEnumFromValue(GetLayoutJustification(), sJustifyKeys);
	else if(pRenderer)
		return pRenderer->GetPropertyS(pProperty);
	return NULL;
}

HKWidget *HKWidget::FindChild(const char *pName)
{
	if(name.CompareInsensitive(pName))
		return this;

	int numChildren = GetNumChildren();
	for(int a=0; a<numChildren; ++a)
	{
		HKWidget *pChild = GetChild(a);
		HKWidget *pFound = pChild->FindChild(pName);
		if(pFound)
			return pFound;
	}

	return NULL;
}

bool HKWidget::SetEnabled(bool bEnable)
{
	bool bOld = bEnabled;
	if(bEnabled != bEnable)
	{
		bEnabled = bEnable;

		if(!OnEnabledChanged.IsEmpty())
		{
			HKWidgetEnabledEvent ev(this, bEnable);
			OnEnabledChanged(*this, ev);
		}
	}
	return bOld;
}

bool HKWidget::SetVisible(bool bVisible)
{
	bool bOld = this->bVisible;
	if(this->bVisible != bVisible)
	{
		this->bVisible = bVisible;

		if(!OnVisibleChanged.IsEmpty())
		{
			HKWidgetVisibilityEvent ev(this, bVisible);
			OnVisibleChanged(*this, ev);
		}
	}
	return bOld;
}

void HKWidget::SetPosition(const MFVector &position)
{
	if(position != pos)
	{
		MFVector oldPos = pos;
		pos = position;

		DirtyMatrices();

		if(!OnMove.IsEmpty())
		{
			HKWidgetMoveEvent ev(this);
			ev.oldPos = oldPos;
			ev.newPos = position;
			OnMove(*this, ev);
		}
	}
}

void HKWidget::SetSize(const MFVector &size)
{
	if(this->size != size)
	{
		MFVector oldSize = this->size;
		this->size = size;

		if(!OnResize.IsEmpty())
		{
			HKWidgetResizeEvent ev(this);
			ev.oldSize = oldSize;
			ev.newSize = size;
			OnResize(*this, ev);
		}
	}
}

void HKWidget::SetColour(const MFVector &colour)
{
	this->colour = colour;
}

void HKWidget::SetScale(const MFVector &scale)
{
	if(this->scale != scale)
	{
		this->scale = scale;

		DirtyMatrices();
	}
}

void HKWidget::SetRotation(const MFVector &rotation)
{
	if(this->rot != rotation)
	{
		this->rot = rotation;

		DirtyMatrices();
	}
}

void HKWidget::SetLayoutMargin(const MFVector &margin)
{
	if(layoutMargin != margin)
	{
		layoutMargin = margin;
		if(pParent)
			pParent->ArrangeChildren();
	}
}

void HKWidget::SetLayoutWeight(float weight)
{
	if(layoutWeight != weight)
	{
		layoutWeight = weight;
		if(pParent)
			pParent->ArrangeChildren();
	}
}

void HKWidget::SetLayoutJustification(Justification justification)
{
	if(layoutJustification != justification)
	{
		layoutJustification = justification;
		if(pParent)
			pParent->ArrangeChildren();
	}
}

void HKWidget::Draw()
{
	if(!bVisible)
		return;

	pRenderer->Render(*this, GetTransform());

	int numChildren = GetNumChildren();
	for(int a=0; a<numChildren; ++a)
	{
		HKWidget *pChild = GetChild(a);
		pChild->Draw();
	}
}

void HKWidget::DirtyMatrices()
{
	bMatrixDirty = bInvMatrixDirty = true;

	int numChildren = GetNumChildren();
	for(int a=0; a<numChildren; ++a)
		GetChild(a)->DirtyMatrices();
}

HKWidget *HKWidget::IntersectWidget(const MFVector &pos, const MFVector &dir, MFVector *pLocalPos)
{
	if(!bVisible)
		return NULL;

	if(size.z == 0.f)
	{
		// the widget is 2d, much easier

		// build a plane from the matrix
		MFVector plane;
		plane = -GetTransform().GetZAxis();

		// normalise if the plane is scaled along z
		if(scale.z != 1.f)
			plane *= 1.f/scale.z;

		// calculate w
		plane.w = -this->pos.Dot3(plane);

		MFRayIntersectionResult res;
		if(MFCollision_RayPlaneTest(pos, dir, plane, &res))
		{
			MFVector intersection = GetInvTransform().TransformVectorH(pos + dir*res.time);

			if(pLocalPos)
				*pLocalPos = intersection;

			MFRect rect =
			{
				0.f, 0.f,
				size.x, size.y
			};
			if(MFTypes_PointInRect(intersection.x, intersection.y, &rect))
			{
				HKWidget *pIntersect = this;

				int numChildren = GetNumChildren();
				for(int a=0; a<numChildren; ++a)
				{
					HKWidget *pChild = GetChild(a)->IntersectWidget(pos, dir, pLocalPos);
					if(pChild)
					{
						pIntersect = pChild;
						break;
					}
				}

				return pIntersect;
			}
		}
	}
	else
	{
		// intersect the 3d widgets cubic boundary
		//...
	}

	return NULL;
}

bool HKWidget::InputEvent(HKInputManager &manager, HKInputManager::EventInfo &ev)
{
	// try and handle the input event in some standard ways...
	switch(ev.ev)
	{
		case HKInputManager::IE_Down:
		{
			HKWidgetInputEvent ie(this, ev.pSource);
			OnDown(*this, ie);
			return true;
		}
		case HKInputManager::IE_Up:
		{
			HKWidgetInputEvent ie(this, ev.pSource);
			OnUp(*this, ie);
			return true;
		}
		case HKInputManager::IE_Tap:
		{
			HKWidgetInputEvent ie(this, ev.pSource);
			OnTap(*this, ie);
			return true;
		}
		case HKInputManager::IE_Hover:
		{
			HKWidgetInputActionEvent ie(this, ev.pSource);
			ie.pos = MakeVector(ev.hover.x, ev.hover.y);
			ie.delta = MakeVector(ev.hover.deltaX, ev.hover.deltaY);
			OnHover(*this, ie);
			return true;
		}
		case HKInputManager::IE_Drag:
		{
			HKWidgetInputActionEvent ie(this, ev.pSource);
			ie.pos = MakeVector(ev.hover.x, ev.hover.y);
			ie.delta = MakeVector(ev.hover.deltaX, ev.hover.deltaY);
			OnDrag(*this, ie);
			return true;
		}
		case HKInputManager::IE_Pinch:
		case HKInputManager::IE_Spin:
		case HKInputManager::IE_ButtonTriggered:
		case HKInputManager::IE_ButtonDown:
		case HKInputManager::IE_ButtonUp:
		case HKInputManager::IE_Wheel:
		default:
			break;
	}

	if(pParent)
		return pParent->InputEvent(manager, ev);
	return false;
}

void HKWidget::ArrangeChildren()
{
}

int HKWidget_GetEnumValue(MFString value, const EnumKeypair *pKeys)
{
	value.Trim();
	for(int a=0; pKeys[a].pKey; ++a)
	{
		if(value.CompareInsensitive(pKeys[a].pKey))
			return pKeys[a].value;
	}
	return -1;
}

uint32 HKWidget_GetBitfieldValue(MFString flags, const EnumKeypair *pKeys)
{
	if(flags.IsEmpty())
		return 0;

	uint32 value = 0;

	int tokLen, offset = 0;
	do
	{
		tokLen = flags.FindChar('|', offset);
		MFString key = flags.SubStr(offset, tokLen);
		key.Trim();

		for(int a=0; pKeys[a].pKey; ++a)
		{
			if(key.CompareInsensitive(pKeys[a].pKey))
			{
				value |= pKeys[a].value;
				break;
			}
		}
		offset += tokLen + 1;
	}
	while(tokLen != -1);

	return value;
}

MFString HKWidget_GetEnumFromValue(int value, const EnumKeypair *pKeys)
{
	for(int a=0; pKeys[a].pKey; ++a)
	{
		if(value == pKeys[a].value)
			return pKeys[a].pKey;
	}
	return NULL;
}

MFString HKWidget_GetBitfieldFromValue(uint32 bits, const EnumKeypair *pKeys)
{
	MFString bitfield = "";

	for(int a=0; a<32; ++a)
	{
		int bit = 1 << a;
		if(!(bits & bit))
			continue;

		for(int b=0; pKeys[b].pKey; ++b)
		{
			if(pKeys[b].value == bit)
			{
				if(!bitfield.IsEmpty())
					bitfield += "|";
				bitfield += pKeys[b].pKey;
				break;
			}
		}
	}
	return bitfield;
}

bool HKWidget_GetBoolFromString(const char *pValue)
{
	if(!MFString_CaseCmp(pValue, "true") ||
		!MFString_CaseCmp(pValue, "1") ||
		!MFString_CaseCmp(pValue, "enabled") ||
		!MFString_CaseCmp(pValue, "on") ||
		!MFString_CaseCmp(pValue, "yes"))
		return true;
	return false;
}

MFVector HKWidget_GetVectorFromString(MFString value)
{
	if(value.IsEmpty())
		return MFVector::zero;

	float f[4] = { 0.f, 0.f, 0.f, 0.f };

	int tokLen, offset = 0;
	int numComponents = 0;
	do
	{
		tokLen = value.FindChar(',', offset);
		MFString key = value.SubStr(offset, tokLen);
		f[numComponents++] = key.ToFloat();
		offset += tokLen + 1;
	}
	while(numComponents < 4 && tokLen != -1);

	if(numComponents == 1)
	{
		f[1] = f[2] = f[3] = f[0];
	}

	return MakeVector(f[0], f[1], f[2], f[3]);
}
