#if !defined(_HKWIDGET_RENDERER_H)
#define _HKWIDGET_RENDERER_H

class HKWidget;
struct MFMaterial;

class HKWidgetRenderer
{
public:
	static HKWidgetRenderer *Create();

	HKWidgetRenderer();
	virtual ~HKWidgetRenderer();

	virtual void Render(const HKWidget &widget, const MFMatrix &worldTransform);

	virtual void SetProperty(const char *pProperty, const char *pValue);
	virtual MFString GetProperty(const char *pProperty);

protected:
	MFVector bgColour;
	MFMaterial *pBackground;
};

#endif
