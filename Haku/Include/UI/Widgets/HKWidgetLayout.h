#if !defined(_HKWIDGET_LAYOUT_H)
#define _HKWIDGET_LAYOUT_H

#include "UI/HKWidget.h"
#include "UI/HKWidgetRenderer.h"

#include "MFArray.h"

class HKWidgetLayout : public HKWidget
{
public:
	enum FitFlags
	{
		FitContentVertical = 1,
		FitContentHorizontal = 2,
	};

	HKWidgetLayout();
	virtual ~HKWidgetLayout();

	int AddChild(HKWidget *pChild, bool bOwnChild = false);
	void RemoveChild(int index);

	virtual int GetNumChildren() const;
	virtual HKWidget *GetChild(int index) const;

	virtual void SetPropertyV(const char *pProperty, const MFVector& value);
	virtual void SetPropertyS(const char *pProperty, const char *pValue);
	virtual const MFVector &GetPropertyV(const char *pProperty);
	virtual MFString GetPropertyS(const char *pProperty);

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

	void OnLayoutDirty(HKWidget &child, HKWidgetEventInfo &ev);

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
