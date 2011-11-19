#pragma once
#if !defined(_HKWIDGET_RENDERER_H)
#define _HKWIDGET_RENDERER_H

#include "HKWidget.h"
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
	MFVector padding;
	MFVector colour;
	MFVector border;		// width: left, top, right, bottom
	MFVector borderColour;
	MFMaterial *pImage;
	HKWidget::Justification imageAlignment;
	float texWidth, texHeight;
	float margin9Cell;
};

#endif
