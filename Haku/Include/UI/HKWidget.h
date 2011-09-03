#if !defined(_HKWIDGET_H)
#define _HKWIDGET_H

#include "HKEvent.h"

// HKWidget is an interactive entity
class HKWidget
{
	friend class HKUserInterface;
	friend class HKWidgetRenderer;
public:
	typedef fastdelegate::FastDelegate1<const HKWidget &> RenderCallback;

	static HKWidget *Create();

	HKWidget();
	~HKWidget();

	virtual void Update();

	const MFMatrix &GetTransform();
	const MFMatrix &GetInvTransform();

	void SetRenderDelegate(RenderCallback renderDelegate);

	bool SetEnabled(bool bEnable);
	bool SetVisible(bool bVisible);

	void SetPosition(const MFVector &position);
	void SetSize(const MFVector &size);
	void SetColour(const MFVector &colour);
	void SetScale(const MFVector &scale);
	void SetRotation(const MFVector &rotation);

	// state change events
	HKEvent OnEnabledChanged;
	HKEvent OnVisibleChanged;

	// interactivity events
	HKEvent OnMove;
	HKEvent OnResize;
	HKEvent OnFocusChanged;

	// input events
	HKEvent OnDown;			// an input source lowered a key. applies to mouse, keyboard, touch, gamepad events
	HKEvent OnUp;			// an input source raised a key. applies to mouse, keyboard, touch, gamepad events
	HKEvent OnClicked;		// a sequence of down followed by an up, without motion in between. applies to mouse, keyboard, touch, gamepad events
	HKEvent OnDrag;			// an input source was moved between a 'down', and 'up' event. applies to mouse, touch events
	HKEvent OnHover;		// an input source moved above a widget. applies to mouse events
	HKEvent OnHoverOver;	// an input source entered the bounds of a widget. applies to mouse events
	HKEvent OnHoverOut;		// an input source left the bounds of a widget. applies to mouse events

	HKEvent OnCharacter;	// if the input was able to generate a unicode character

protected:
	MFVector pos;			// relative to parent
	MFVector size;			// size of widget volume
	MFVector colour;		// colour modulation
	MFVector scale;			// scale the widget
	MFVector rot;			// rotation of the widget

	MFMatrix matrix;
	MFMatrix invMatrix;

	RenderCallback renderCallback;

	const char *pTypeName;

	HKWidget *pParent;

	bool bVisible;
	bool bEnabled;
	bool bParentEnabled;	// flagged if the parent is enabled

	int zDepth;

	bool bMatrixDirty, bInvMatrixDirty;

	virtual bool IntersectWidget(const MFVector &pos, const MFVector &dir);	// test for ray intersecting the widget
};

#endif
