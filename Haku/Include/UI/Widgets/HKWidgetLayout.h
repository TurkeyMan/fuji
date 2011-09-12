#if !defined(_HKWIDGET_LAYOUT_H)
#define _HKWIDGET_LAYOUT_H

#include "UI/HKWidget.h"
#include "UI/HKWidgetRenderer.h"

#include "MFArray.h"

class HKWidgetLayout : public HKWidget
{
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

	enum FitFlags
	{
		FitContentVertical = 1,
		FitContentHorizontal = 2,
	};

	HKWidgetLayout();
	~HKWidgetLayout();

	int AddChild(HKWidget *pChild, Justification justification = None, float weight = 1.f, const MFVector &margin = MFVector::zero);
	void RemoveChild(int index);

	virtual int GetNumChildren() const;
	virtual HKWidget *GetChild(int index) const;

	void SetMargin(const MFVector &margin);
	void SetFitFlags(uint32 fitFlags);

	void SetChildWeight(int index, float weight);
	void SetChildMargin(int index, const MFVector &margin);
	void SetChildJustification(int index, Justification justification);

	const MFVector &GetMargin() const { return margin; }

	float GetChildWeight(int index) const { return children[index].weight; }
	const MFVector &GetChildMargin(int index) const { return children[index].margin; }
	Justification GetChildJustification(int index) const { return children[index].justification; }

protected:
	struct Child
	{
		MFVector margin;
		HKWidget *pChild;
		Justification justification;
		float weight;
	};

	MFVector margin;
	uint32 fitFlags;

	MFArray<Child> children;

	virtual void ArrangeChildren() = 0;

	void OnLayoutDirty(HKWidget &child, HKWidgetEventInfo &ev);
};

class HKWidgetRendererLayout : public HKWidgetRenderer
{
public:
	static HKWidgetRenderer *Create();
protected:
	virtual void Render(const HKWidget &widget, const MFMatrix &worldTransform);
};

#endif
