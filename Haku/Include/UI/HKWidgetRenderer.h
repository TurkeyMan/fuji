#if !defined(_HKWIDGET_RENDERER_H)
#define _HKWIDGET_RENDERER_H

class HKWidget;

class HKWidgetRenderer
{
public:
	virtual void Render(const HKWidget &widget, const MFMatrix &worldTransform) = 0;
};

#endif
