#if !defined(_HKWIDGET_LAYOUT_H)
#define _HKWIDGET_LAYOUT_H

#include "UI/HKWidget.h"
#include "UI/HKWidgetRenderer.h"

#include "Fuji/MFArray.h"

class HKWidgetLayout : public HKWidget
{
public:
	enum FitFlags
	{
		FitContentVertical = 1,
		FitContentHorizontal = 2,
	};

	HKWidgetLayout(HKWidgetType *pType);
	virtual ~HKWidgetLayout();

	int AddChild(HKWidget *pChild, bool bOwnChild = false);
	void RemoveChild(HKWidget *pChild);
	void RemoveChild(int index);

	void ClearChildren();

	virtual int GetNumChildren() const;
	virtual HKWidget *GetChild(int index) const;

	virtual void SetProperty(const char *pProperty, const char *pValue);
	virtual MFString GetProperty(const char *pProperty);

	void SetPadding(const MFVector &padding);
	void SetFitFlags(uint32 fitFlags);

	const MFVector &GetPadding() const { return padding; }

protected:
	struct Child
	{
		HKWidget *pChild;
		bool bOwnChild;
	};

	MFVector padding;
	uint32 fitFlags;

	MFArray<Child> children;

	void OnLayoutDirty(HKWidget &child, const HKWidgetEventInfo &ev);

	virtual void ArrangeChildren() = 0;
	void ResizeChild(HKWidget *pChild, const MFVector &newSize) { pChild->Resize(newSize); }

	static const EnumKeypair sFirFlagsKeys[];
};

class HKWidgetRendererLayout : public HKWidgetRenderer
{
public:
	static HKWidgetRenderer *Create();
protected:
	virtual void Render(const HKWidget &widget, const MFMatrix &worldTransform);
};

#endif
