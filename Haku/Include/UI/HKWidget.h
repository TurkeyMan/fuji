#if !defined(_HKWIDGET_H)
#define _HKWIDGET_H

// HKBaseWidget can exist in a hierarchy, and be enabled/disabled
class HKBaseWidget
{
public:
	HKBaseWidget();
	~HKBaseWidget();

	bool SetEnabled(bool bEnable);

	HKEvent OnEnabledChanged;

protected:
	bool bEnabled;

	HKWidget *pParent;
};

// HKWidget is a visible entity
class HKWidget : public HKBaseWidget
{
public:
	HKWidget();
	~HKWidget();

	void SetPosition(const MFVector &position);
	void SetSize(const MFVector &size);
	void SetColour(const MFVector &colour);
	void SetScale(const MFVector &scale);
	void SetRotation(const MFVector &rotation);

	bool SetVisible(bool bEnable);

	// standard events
	HKEvent OnMove;
	HKEvent OnResize;
	HKEvent OnVisibleChanged;
	HKEvent OnFocusChanged;

	// input events
	HKEvent OnDown;			// an input source lowered a key. applies to mouse, keyboard, touch, gamepad events
	HKEvent OnUp;			// an input source raised a key. applies to mouse, keyboard, touch, gamepad events
	HKEvent OnClicked;		// a sequence of down followed by an up, without motion in between. applies to mouse, keyboard, touch, gamepad events
	HKEvent OnDrag;			// an input source was moved between a 'down', and 'up' event. applies to mouse, touch events
	HKEvent OnHover;		// an input source moved above a widget. applies to mouse events
	HKEvent OnHoverOver;	// an input source entered the bounds of a widget. applies to mouse events
	HKEvent OnHoverOut;		// an input source left the bounds of a widget. applies to mouse events

protected:
	MFVector pos;			// relative to parent
	MFVector size;			// size of widget volume
	MFVector colour;		// colour modulation
	MFVector scale;			// scale the widget
	MFVector rotation;		// rotation of the widget

	MFMatrix matrix;
	MFMatrix invMatrix;

	int zDepth;

	bool bVisible;
	bool bParentEnabled;	// flagged if the parent is enabled

	bool bMatrixDirty, bInvMatrixDirty;

	virtual IntersectWidget();	// test for ray intersecting the widget
};

#endif
