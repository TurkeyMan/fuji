#pragma once
#if !defined(_HKWIDGET_RENDERER_H)
#define _HKWIDGET_RENDERER_H

#include "HKFactory.h"

class HKWidgetRenderer;
typedef HKFactory<HKWidgetRenderer> HKWidgetRendererFactory;
typedef HKWidgetRendererFactory::FactoryType HKWidgetRendererType;

class HKWidget;
struct MFMaterial;

class HKWidgetRenderer
{
public:
	static HKWidgetRenderer *Create(HKWidgetRendererType *pType);

	HKWidgetRenderer();
	virtual ~HKWidgetRenderer();

	virtual void Render(const HKWidget &widget, const MFMatrix &worldTransform);

	virtual bool SetProperty(const char *pProperty, const char *pValue);
	virtual MFString GetProperty(const char *pProperty);

protected:
	MFVector bgColour;
	MFMaterial *pBackground;
};

#endif
