#pragma once
#if !defined(_HKWIDGET_H)
#define _HKWIDGET_H

#include "HKWidgetEvent.h"
#include "HKInputSource.h"
#include "HKFactory.h"

class HKWidget;
class HKWidgetRenderer;
typedef HKFactory<HKWidget> HKWidgetFactory;
typedef HKWidgetFactory::FactoryType HKWidgetType;

class HKUserInterface;

struct EnumKeypair
{
	const char *pKey;
	int value;
};

bool HKWidget_GetBoolFromString(const char *pValue);
MFVector HKWidget_GetVectorFromString(MFString value);
MFVector HKWidget_GetColourFromString(MFString value);
int HKWidget_GetEnumValue(MFString value, const EnumKeypair *pKeys);
uint32 HKWidget_GetBitfieldValue(MFString flags, const EnumKeypair *pKeys);

MFString HKWidget_GetEnumFromValue(int value, const EnumKeypair *pKeys);
MFString HKWidget_GetBitfieldFromValue(uint32 bits, const EnumKeypair *pKeys);

void HKWidget_BindWidgetEvent(HKWidgetEvent &event, const char *pEventName);

// HKWidget is an interactive entity
class HKWidget
{
	friend class HKUserInterface;
	friend class HKWidgetLayout;
	friend class HKWidgetRenderer;
public:
	enum Align
	{
		Align_None = -1,

		Align_Left = 0,
		Align_Center,
		Align_Right,
		Align_Fill,

		Align_Max
	};

	enum VAlign
	{
		VAlign_None = -1,

		VAlign_Top = 0,
		VAlign_Center,
		VAlign_Bottom,
		VAlign_Fill,

		VAlign_Max
	};

	enum Justification
	{
		TopLeft = 0,
		TopCenter,
		TopRight,
		TopFill,
		CenterLeft,
		Center,
		CenterRight,
		CenterFill,
		BottomLeft,
		BottomCenter,
		BottomRight,
		BottomFill,
		FillLeft,
		FillCenter,
		FillRight,
		Fill,

		None,

		JustifyMax
	};

	enum Visibility
	{
		Visible = 0,
		Invisible,
		Gone
	};

	static HKWidget *Create(HKWidgetType *pType);
	static const char *TypeName() { return "Widget"; }

	HKWidget(HKWidgetType *pType);
	virtual ~HKWidget();

	HKUserInterface &GetUI() const;

	void SetRenderer(HKWidgetRenderer *pRenderer);
	HKWidgetRenderer *GetRenderer() const { return pRenderer; }

	const char *GetTypeName() const { return pType->typeName; }
	bool IsType(const char *pType) const;

	void SetUserData(void *pUserData) { this->pUserData = pUserData; }
	void *GetUserData() const { return pUserData; }

	// support widget hierarchy
	virtual int GetNumChildren() const;
	virtual HKWidget *GetChild(int index) const;

	HKWidget *FindChild(const char *pName);
	template<typename T> T *FindChild(const char *pName);

	HKWidget *GetParent() const { return pParent; }

	// arbitrary state configuration
	virtual void SetProperty(const char *pProperty, const char *pValue);
	virtual MFString GetProperty(const char *pProperty);

	// HKWidget accessor methods
	MFString GetID() const { return id; }

	MFString GetStyle() const { return style; }

	bool IsEnabled() const { return bEnabled && bParentEnabled; }

	bool IsClickable() const { return bClickable; }
	bool IsDragable() const { return bDragable; }

	bool GetEnabled() const { return bEnabled; }
	Visibility GetVisible() const { return visible; }

	const MFVector &GetPosition() const { return pos; }
	const MFVector &GetSize() const { return size; }
	const MFVector &GetColour() const { return colour; }
	const MFVector &GetScale() const { return scale; }
	const MFVector &GetRotation() const { return rot; }

	const MFMatrix &GetTransform();
	const MFMatrix &GetInvTransform();

	const MFVector &GetLayoutMargin() const { return layoutMargin; }
	float GetLayoutWeight() const { return layoutWeight; }
	Justification GetLayoutJustification() const { return layoutJustification; }
	Align GetHAlign() const { return layoutJustification == None ? Align_None : (Align)(layoutJustification & 3); }
	VAlign GetVAlign() const { return layoutJustification == None ? VAlign_None : (VAlign)((layoutJustification >> 2) & 3); } 

	MFVector GetSizeWithMargin() const { return MakeVector(size.x + layoutMargin.x + layoutMargin.z, size.y + layoutMargin.y + layoutMargin.w, size.z, size.w); }

	void SetID(MFString id) { this->id = id; }

	void SetStyle(MFString style);
	void SetStyleDisabled(MFString style);

	bool SetEnabled(bool bEnable);
	Visibility SetVisible(Visibility visible);

	bool SetClickable(bool bClickable) { bool bOld = this->bClickable; this->bClickable = bClickable; return bOld; }
	bool SetDragable(bool bDragable) { bool bOld = this->bDragable; this->bDragable = bDragable; return bOld; }
	bool SetHoverable(bool bHoverable) { bool bOld = this->bHoverable; this->bHoverable = bHoverable; return bOld; }

	void SetPosition(const MFVector &position);
	void SetSize(const MFVector &size) { bAutoWidth = bAutoHeight = false; Resize(size); }
	void SetWidth(float width) { bAutoWidth = false; UpdateWidth(width); }
	void SetHeight(float height) { bAutoHeight = false; UpdateHeight(height); }
	void SetColour(const MFVector &colour);
	void SetScale(const MFVector &scale);
	void SetRotation(const MFVector &rotation);

	void SetLayoutMargin(const MFVector &margin);
	void SetLayoutWeight(float weight);
	void SetLayoutJustification(Justification justification);

	// state change events
	HKWidgetEvent OnEnabledChanged;
	HKWidgetEvent OnVisibleChanged;

	HKWidgetEvent OnLayoutChanged;

	// interactivity events
	HKWidgetEvent OnMove;
	HKWidgetEvent OnResize;
	HKWidgetEvent OnFocusChanged;

	// input events
	HKWidgetEvent OnDown;		// an input source lowered a key. applies to mouse, keyboard, touch, gamepad events
	HKWidgetEvent OnUp;			// an input source raised a key. applies to mouse, keyboard, touch, gamepad events
	HKWidgetEvent OnTap;		// a sequence of down followed by an up, without motion in between. applies to mouse, keyboard, touch, gamepad events
	HKWidgetEvent OnDrag;		// an input source was moved between a 'down', and 'up' event. applies to mouse, touch events
	HKWidgetEvent OnHover;		// an input source moved above a widget. applies to mouse events
	HKWidgetEvent OnHoverOver;	// an input source entered the bounds of a widget. applies to mouse events
	HKWidgetEvent OnHoverOut;	// an input source left the bounds of a widget. applies to mouse events

	HKWidgetEvent OnCharacter;	// if the input was able to generate a unicode character

protected:
	MFVector pos;			// relative to parent
	MFVector size;			// size of widget volume
	MFVector colour;		// colour modulation
	MFVector scale;			// scale the widget
	MFVector rot;			// rotation of the widget

	MFVector layoutMargin;	// margin surrounding the widget within its container

	MFMatrix matrix;
	MFMatrix invMatrix;

	HKWidgetType *pType;
	HKWidget *pParent;
	HKWidgetRenderer *pRenderer;

	void *pUserData;

	MFString id;

	MFString style;
	MFString styleDisabled;

	Visibility visible;
	bool bEnabled;
	bool bParentEnabled;	// flagged if the parent is enabled

	bool bAutoWidth, bAutoHeight;
	bool bClickable, bDragable, bHoverable;

	int zDepth;

	Justification layoutJustification;
	float layoutWeight;

	bool bMatrixDirty, bInvMatrixDirty;

	virtual void Update();
	void Draw();

	void ApplyStyle(MFString style);
	virtual void UpdateStyle();

	void DirtyMatrices();

	void Resize(const MFVector &size);
	void UpdateWidth(float width) { MFVector newSize = GetSize(); newSize.x = width; Resize(newSize); }
	void UpdateHeight(float height) { MFVector newSize = GetSize(); newSize.y = height; Resize(newSize); }

	virtual HKWidget *IntersectWidget(const MFVector &pos, const MFVector &dir, MFVector *pLocalPos);	// test for ray intersecting the widget
	virtual bool InputEvent(HKInputManager &manager, const HKInputManager::EventInfo &ev);

	static const EnumKeypair sJustifyKeys[];
	static const EnumKeypair sVisibilityKeys[];
};

template<typename T>
inline T *HKWidget::FindChild(const char *pName)
{
	HKWidget *pWidget = FindChild(pName);
	if(!pWidget)
		return NULL;

	bool bOfType = pWidget->IsType(T::TypeName());
	MFDebug_Assert(bOfType, MFStr("Widget id='%s' is not of expected type '%s'! (%s)", pName, T::TypeName(), pWidget->GetTypeName()));

	if(!bOfType)
		return NULL;
	return (T*)pWidget;
}

#endif
