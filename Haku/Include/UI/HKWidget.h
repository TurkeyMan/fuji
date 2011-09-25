#if !defined(_HKWIDGET_H)
#define _HKWIDGET_H

#include "HKWidgetEvent.h"
#include "HKWidgetRenderer.h"
#include "HKInputSource.h"

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

// HKWidget is an interactive entity
class HKWidget
{
	friend class HKUserInterface;
	friend class HKWidgetLayout;
public:
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

	static HKWidget *Create();

	HKWidget();
	virtual ~HKWidget();

	HKUserInterface &GetUI();

	void SetRenderer(HKWidgetRenderer *pRenderer);

	// support widget hierarchy
	virtual int GetNumChildren() const;
	virtual HKWidget *GetChild(int index) const;

	HKWidget *FindChild(const char *pName);

	HKWidget *GetParent() const { return pParent; }

	// arbitrary state configuration
	virtual void SetProperty(const char *pProperty, const char *pValue);
	virtual MFString GetProperty(const char *pProperty);

	// HKWidget accessor methods
	MFString GetName() const { return name; }
	const char *GetTypeName() const { return pTypeName; }

	bool IsEnabled() const { return bEnabled && bParentEnabled; }

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

	void SetName(MFString name) { this->name = name; }

	bool SetEnabled(bool bEnable);
	Visibility SetVisible(Visibility visible);

	void SetPosition(const MFVector &position);
	void SetSize(const MFVector &size);
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

	HKWidgetRenderer *pRenderer;
	HKWidget *pParent;

	MFString name;

	const char *pTypeName;

	Visibility visible;
	bool bEnabled;
	bool bParentEnabled;	// flagged if the parent is enabled

	bool bAutoSize;

	int zDepth;

	Justification layoutJustification;
	float layoutWeight;

	bool bMatrixDirty, bInvMatrixDirty;

	virtual void Update();
	void Draw();

	void DirtyMatrices();

	virtual HKWidget *IntersectWidget(const MFVector &pos, const MFVector &dir, MFVector *pLocalPos);	// test for ray intersecting the widget
	virtual bool InputEvent(HKInputManager &manager, HKInputManager::EventInfo &ev);

	static const EnumKeypair sJustifyKeys[];
	static const EnumKeypair sVisibilityKeys[];
};

#endif
