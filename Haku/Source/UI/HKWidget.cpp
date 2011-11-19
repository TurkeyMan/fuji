#include "Haku.h"
#include "UI/HKWidget.h"
#include "UI/HKUI.h"
#include "UI/HKWidgetStyle.h"

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
	{ "None", None },
	{ NULL, 0 }
};

const EnumKeypair HKWidget::sVisibilityKeys[] =
{
	{ "Visible", Visible },
	{ "Invisible", Invisible },
	{ "Gone", Gone },
	{ NULL, 0 }
};

HKWidget::HKWidget(HKWidgetType *_pType)
{
	pType = _pType;

	pos = MFVector::zero;
	size = MFVector::zero;
	colour = MFVector::white;
	scale = MFVector::one;
	rot = MFVector::zero;

	layoutMargin = MFVector::zero;
	layoutJustification = None;
	layoutWeight = 1.f;

	pRenderer = NULL;
	pParent = NULL;

	pUserData = NULL;

	zDepth = 0;

	visible = Visible;
	bEnabled = true;
	bParentEnabled = true;

	bAutoWidth = bAutoHeight = true;
	bClickable = bDragable = bHoverable = false;
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

HKWidget *HKWidget::Create(HKWidgetType *pType)
{
	return new HKWidget(pType);
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

HKUserInterface &HKWidget::GetUI() const
{
	return HKUserInterface::Get();
}

bool HKWidget::IsType(const char *_pType) const
{
	HKWidgetType *pT = pType;
	while(pT)
	{
		if(!MFString_Compare(_pType, pType->typeName))
			return true;
		pT = pT->pParent;
	}
	return false;
}

int HKWidget::GetNumChildren() const
{
	return 0;
}

HKWidget *HKWidget::GetChild(int index) const
{
	return NULL;
}

void HKWidget::SetProperty(const char *pProperty, const char *pValue)
{
	if(!MFString_CaseCmp(pProperty, "style"))
		SetStyle(pValue);
	else if(!MFString_CaseCmp(pProperty, "style_disabled"))
		SetStyleDisabled(pValue);
	else if(!MFString_CaseCmp(pProperty, "enabled"))
		SetEnabled(HKWidget_GetBoolFromString(pValue));
	else if(!MFString_CaseCmp(pProperty, "visible"))
		SetVisible((Visibility)HKWidget_GetEnumValue(pValue, sVisibilityKeys));
	else if(!MFString_CaseCmp(pProperty, "zDepth"))
		zDepth = MFString_AsciiToInteger(pValue);
	else if(!MFString_CaseCmp(pProperty, "weight"))
		SetLayoutWeight(MFString_AsciiToFloat(pValue));
	else if(!MFString_CaseCmp(pProperty, "position"))
		SetPosition(HKWidget_GetVectorFromString(pValue));
	else if(!MFString_CaseCmp(pProperty, "size"))
		SetSize(HKWidget_GetVectorFromString(pValue));
	else if(!MFString_CaseCmp(pProperty, "width"))
		SetWidth(MFString_AsciiToFloat(pValue));
	else if(!MFString_CaseCmp(pProperty, "height"))
		SetHeight(MFString_AsciiToFloat(pValue));
	else if(!MFString_CaseCmp(pProperty, "scale"))
		SetScale(HKWidget_GetVectorFromString(pValue));
	else if(!MFString_CaseCmp(pProperty, "colour"))
		SetColour(HKWidget_GetColourFromString(pValue));
	else if(!MFString_CaseCmp(pProperty, "rotation"))
		SetRotation(HKWidget_GetVectorFromString(pValue));
	else if(!MFString_CaseCmp(pProperty, "margin"))
		SetLayoutMargin(HKWidget_GetVectorFromString(pValue));
	else if(!MFString_CaseCmp(pProperty, "id"))
		id = pValue;
	else if(!MFString_CaseCmp(pProperty, "align"))
		SetLayoutJustification((Justification)HKWidget_GetEnumValue(pValue, sJustifyKeys));
	else if(!MFString_CaseCmp(pProperty, "onEnabledChanged"))
		HKWidget_BindWidgetEvent(OnEnabledChanged, pValue);
	else if(!MFString_CaseCmp(pProperty, "onVisibleChanged"))
		HKWidget_BindWidgetEvent(OnVisibleChanged, pValue);
	else if(!MFString_CaseCmp(pProperty, "onLayoutChanged"))
		HKWidget_BindWidgetEvent(OnLayoutChanged, pValue);
	else if(!MFString_CaseCmp(pProperty, "onMove"))
		HKWidget_BindWidgetEvent(OnMove, pValue);
	else if(!MFString_CaseCmp(pProperty, "onResize"))
		HKWidget_BindWidgetEvent(OnResize, pValue);
	else if(!MFString_CaseCmp(pProperty, "onFocusChanged"))
		HKWidget_BindWidgetEvent(OnFocusChanged, pValue);
	else if(!MFString_CaseCmp(pProperty, "onDown"))
		HKWidget_BindWidgetEvent(OnDown, pValue);
	else if(!MFString_CaseCmp(pProperty, "onUp"))
		HKWidget_BindWidgetEvent(OnUp, pValue);
	else if(!MFString_CaseCmp(pProperty, "onTap"))
		HKWidget_BindWidgetEvent(OnTap, pValue);
	else if(!MFString_CaseCmp(pProperty, "onDrag"))
		HKWidget_BindWidgetEvent(OnDrag, pValue);
	else if(!MFString_CaseCmp(pProperty, "onHover"))
		HKWidget_BindWidgetEvent(OnHover, pValue);
	else if(!MFString_CaseCmp(pProperty, "onHoverOver"))
		HKWidget_BindWidgetEvent(OnHoverOver, pValue);
	else if(!MFString_CaseCmp(pProperty, "onHoverOut"))
		HKWidget_BindWidgetEvent(OnHoverOut, pValue);
	else if(!MFString_CaseCmp(pProperty, "onCharacter"))
		HKWidget_BindWidgetEvent(OnCharacter, pValue);
	else if(pRenderer && pRenderer->SetProperty(pProperty, pValue))
		return;
	else
		MFDebug_Warn(2, MFString::Format("Unknown property for '%s': %s='%s'", GetTypeName(), pProperty, pValue).CStr());
}

MFString HKWidget::GetProperty(const char *pProperty)
{
	if(!MFString_CaseCmp(pProperty, "id"))
		return id;
	else if(!MFString_CaseCmp(pProperty, "align"))
		return HKWidget_GetEnumFromValue(GetLayoutJustification(), sJustifyKeys);
	else if(pRenderer)
		return pRenderer->GetProperty(pProperty);
	return NULL;
}

HKWidget *HKWidget::FindChild(const char *pName)
{
	int numChildren = GetNumChildren();

	for(int a=0; a<numChildren; ++a)
	{
		HKWidget *pChild = GetChild(a);
		if(pChild->GetID().EqualsInsensitive(pName))
			return pChild;
	}

	for(int a=0; a<numChildren; ++a)
	{
		HKWidget *pChild = GetChild(a);
		HKWidget *pFound = pChild->FindChild(pName);
		if(pFound)
			return pFound;
	}

	return NULL;
}

void HKWidget::SetStyle(MFString style)
{
	if(this->style == style)
		return;

	this->style = style;

	if(IsEnabled() || styleDisabled.IsEmpty())
		ApplyStyle(style);
}

void HKWidget::SetStyleDisabled(MFString style)
{
	if(styleDisabled == style)
		return;

	styleDisabled = style;

	if(!IsEnabled())
		ApplyStyle(style);
}

bool HKWidget::SetEnabled(bool bEnable)
{
	bool bOld = bEnabled;
	if(bEnabled != bEnable)
	{
		bEnabled = bEnable;

		UpdateStyle();

		if(!OnEnabledChanged.IsEmpty())
		{
			HKWidgetEnabledEvent ev(this, bEnable);
			OnEnabledChanged(*this, ev);
		}
	}
	return bOld;
}

HKWidget::Visibility HKWidget::SetVisible(Visibility visible)
{
	Visibility old = this->visible;
	if(this->visible != visible)
	{
		this->visible = visible;

		if(old == Gone || visible == Gone)
		{
			HKWidgetGeneralEvent ev(this);
			OnLayoutChanged(*this, ev);
		}

		if(!OnVisibleChanged.IsEmpty())
		{
			HKWidgetVisibilityEvent ev(this, visible);
			OnVisibleChanged(*this, ev);
		}
	}
	return old;
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

void HKWidget::Resize(const MFVector &size)
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

		HKWidgetGeneralEvent ev(this);
		OnLayoutChanged(*this, ev);
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

		HKWidgetGeneralEvent ev(this);
		OnLayoutChanged(*this, ev);
	}
}

void HKWidget::SetLayoutWeight(float weight)
{
	if(layoutWeight != weight)
	{
		layoutWeight = weight;

		HKWidgetGeneralEvent ev(this);
		OnLayoutChanged(*this, ev);
	}
}

void HKWidget::SetLayoutJustification(Justification justification)
{
	if(layoutJustification != justification)
	{
		layoutJustification = justification;

		HKWidgetGeneralEvent ev(this);
		OnLayoutChanged(*this, ev);
	}
}

void HKWidget::ApplyStyle(MFString style)
{
	if(style.IsEmpty())
		return;

	HKWidgetStyle *pStyle = HKWidgetStyle::FindStyle(style.CStr());
	if(pStyle)
		pStyle->Apply(this);
}

void HKWidget::UpdateStyle()
{
	if(!bEnabled && !styleDisabled.IsEmpty())
		ApplyStyle(styleDisabled);
	else if(!style.IsEmpty())
		ApplyStyle(style);
}

void HKWidget::Draw()
{
	if(GetVisible() != Visible)
		return;

	if(pRenderer)
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
	if(GetVisible() != Visible)
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
				for(int a=numChildren-1; a>=0; --a)
				{
					MFVector childLocal;
					HKWidget *pChild = GetChild(a)->IntersectWidget(pos, dir, &childLocal);
					if(pChild)
					{
						if(pLocalPos)
							*pLocalPos = childLocal;

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

bool HKWidget::InputEvent(HKInputManager &manager, const HKInputManager::EventInfo &ev)
{
	// try and handle the input event in some standard ways...
	switch(ev.ev)
	{
		case HKInputManager::IE_Down:
		{
			if(bClickable)
			{
				HKWidgetInputEvent ie(this, ev.pSource);
				OnDown(*this, ie);
				return true;
			}
			break;
		}
		case HKInputManager::IE_Up:
		{
			if(bClickable)
			{
				HKWidgetInputEvent ie(this, ev.pSource);
				OnUp(*this, ie);
				return true;
			}
			break;
		}
		case HKInputManager::IE_Tap:
		{
			if(bClickable)
			{
				HKWidgetInputEvent ie(this, ev.pSource);
				OnTap(*this, ie);
				return true;
			}
			break;
		}
		case HKInputManager::IE_Hover:
		{
			if(bHoverable)
			{
				HKWidgetInputActionEvent ie(this, ev.pSource);
				ie.pos = MakeVector(ev.hover.x, ev.hover.y);
				ie.delta = MakeVector(ev.hover.deltaX, ev.hover.deltaY);
				OnHover(*this, ie);
				return true;
			}
			break;
		}
		case HKInputManager::IE_Drag:
		{
			if(bDragable)
			{
				HKWidgetInputActionEvent ie(this, ev.pSource);
				ie.pos = MakeVector(ev.hover.x, ev.hover.y);
				ie.delta = MakeVector(ev.hover.deltaX, ev.hover.deltaY);
				OnDrag(*this, ie);
				return true;
			}
			break;
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

int HKWidget_GetEnumValue(MFString value, const EnumKeypair *pKeys)
{
	value.Trim();
	for(int a=0; pKeys[a].pKey; ++a)
	{
		if(value.EqualsInsensitive(pKeys[a].pKey))
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
			if(key.EqualsInsensitive(pKeys[a].pKey))
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

MFVector HKWidget_GetColourFromString(MFString value)
{
	if(value.IsEmpty())
		return MFVector::white;

	if(value.BeginsWith("$") || value.BeginsWith("0x"))
	{
		MFDebug_Assert(false, "Hex colours not supported... pester manu!");
		return MFVector::white;
	}

	if(value.EqualsInsensitive("black"))
		return MFVector::black;
	else if(value.EqualsInsensitive("white"))
		return MFVector::white;
	else if(value.EqualsInsensitive("red"))
		return MFVector::red;
	else if(value.EqualsInsensitive("blue"))
		return MFVector::blue;
	else if(value.EqualsInsensitive("green"))
		return MFVector::green;
	else if(value.EqualsInsensitive("yellow"))
		return MFVector::yellow;
	else if(value.EqualsInsensitive("orange"))
		return MakeVector(1,0.5,0,1);
	else if(value.EqualsInsensitive("grey"))
		return MFVector::grey;
	else if(value.EqualsInsensitive("lightgrey"))
		return MFVector::lightgrey;
	else if(value.EqualsInsensitive("darkgrey"))
		return MFVector::darkgrey;

	float f[4] = { 0.f, 0.f, 0.f, 1.f };

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

	return MakeVector(f[0], f[1], f[2], f[3]);
}

void HKWidget_BindWidgetEvent(HKWidgetEvent &event, const char *pEventName)
{
	HKWidgetEvent::Delegate &d = HKUserInterface::GetEventHandler(pEventName);
	if(&d != NULL)
		event += d;
}
