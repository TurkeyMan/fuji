#if !defined(_HKWIDGET_LABEL_H)
#define _HKWIDGET_LABEL_H

#include "UI/HKWidget.h"
#include "UI/HKWidgetRenderer.h"

#include "MFFont.h"

class HKWidgetLabel : public HKWidget
{
public:
	static HKWidget *Create();

	HKWidgetLabel();
	~HKWidgetLabel();

	MFString GetLabel() const { return label; }
	MFFont *GetFont() const { return pFont; }
	MFFontJustify GetTextJustification() const { return justification; }
	float GetTextHeight() const { return textHeight; }

	void SetLabel(MFString label);
	void SetFont(MFFont *pFont) { this->pFont = pFont; }
	void SetTextJustification(MFFontJustify justification) { this->justification = justification; }
	void SetTextHeight(float height) { this->textHeight = height; }

protected:
	MFString label;

	MFFont *pFont;
	MFFontJustify justification;

	float textHeight;
};

class HKWidgetRendererLabel : public HKWidgetRenderer
{
public:
	static HKWidgetRenderer *Create();
protected:
	virtual void Render(const HKWidget &widget, const MFMatrix &worldTransform);
};

#endif
