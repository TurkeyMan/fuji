#if !defined(_HKWIDGET_RENDERER_H)
#define _HKWIDGET_RENDERER_H

#include "fastdelegate/FastDelegate.h"

class HKWidget;

class HKWidgetRenderer
{
public:
	typedef fastdelegate::FastDelegate1<const HKWidget &> RenderCallback;

	static HKWidgetRenderer *GetRenderer(const char *pWidget);

	RenderCallback GetRenderDelegate() { return fastdelegate::MakeDelegate(this, &HKWidgetRenderer::Render); }

protected:
	virtual void Render(const HKWidget &widget) = 0;
};

#endif
