#if !defined(_HKWIDGET_H)
#define _HKWIDGET_H

#include "HKWidgetEvent.h"
#include "HKWidgetRenderer.h"

// HKWidget is an interactive entity
class HKWidget
{
	friend class HKUserInterface;
	friend class HKWidgetRenderer;
public:
	static HKWidget *Create();

	HKWidget();
	~HKWidget();

	virtual void Update();

	void SetRenderer(HKWidgetRenderer *pRenderer);

	bool IsEnabled() const { return bEnabled && bParentEnabled; }

	bool GetEnabled() const { return bEnabled; }
	bool GetVisible() const { return bVisible; }

	const MFVector &GetPosition() const { return pos; }
	const MFVector &GetSize() const { return size; }
	const MFVector &GetColour() const { return colour; }
	const MFVector &GetScale() const { return scale; }
	const MFVector &GetRotation() const { return rot; }

	const MFMatrix &GetTransform();
	const MFMatrix &GetInvTransform();

	bool SetEnabled(bool bEnable);
	bool SetVisible(bool bVisible);

	void SetPosition(const MFVector &position);
	void SetSize(const MFVector &size);
	void SetColour(const MFVector &colour);
	void SetScale(const MFVector &scale);
	void SetRotation(const MFVector &rotation);

	// state change events
	HKWidgetEvent OnEnabledChanged;
	HKWidgetEvent OnVisibleChanged;

	// interactivity events
	HKWidgetEvent OnMove;
	HKWidgetEvent OnResize;
	HKWidgetEvent OnFocusChanged;

	// input events
	HKWidgetEvent OnDown;			// an input source lowered a key. applies to mouse, keyboard, touch, gamepad events
	HKWidgetEvent OnUp;			// an input source raised a key. applies to mouse, keyboard, touch, gamepad events
	HKWidgetEvent OnTap;			// a sequence of down followed by an up, without motion in between. applies to mouse, keyboard, touch, gamepad events
	HKWidgetEvent OnDrag;			// an input source was moved between a 'down', and 'up' event. applies to mouse, touch events
	HKWidgetEvent OnHover;		// an input source moved above a widget. applies to mouse events
	HKWidgetEvent OnHoverOver;	// an input source entered the bounds of a widget. applies to mouse events
	HKWidgetEvent OnHoverOut;		// an input source left the bounds of a widget. applies to mouse events

	HKWidgetEvent OnCharacter;	// if the input was able to generate a unicode character

protected:
	MFVector pos;			// relative to parent
	MFVector size;			// size of widget volume
	MFVector colour;		// colour modulation
	MFVector scale;			// scale the widget
	MFVector rot;			// rotation of the widget

	MFMatrix matrix;
	MFMatrix invMatrix;

	HKWidgetRenderer::RenderCallback renderCallback;
	HKWidgetRenderer *pRenderer;

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
