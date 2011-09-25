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
	virtual ~HKWidgetLabel();

	virtual void SetProperty(const char *pProperty, const char *pValue);
	virtual MFString GetProperty(const char *pProperty);

	MFString GetText() const { return text; }
	MFString GetFontName() const { return font; }
	MFFont *GetFont() const { return pFont; }
	const MFVector &GetTextColour() { return textColour; }
	MFFontJustify GetTextJustification() const { return textJustification; }
	float GetTextHeight() const { return textHeight; }
	float GetShadowDepth() const { return shadowDepth; }

	void LoadFont(MFString font);

	void SetText(MFString Text);
	void SetFont(MFFont *pFont);
	void SetTextColour(const MFVector &colour) { textColour = colour; }
	void SetTextJustification(MFFontJustify justification) { this->textJustification = justification; }
	void SetTextHeight(float height) { this->textHeight = height; bAutoTextHeight = false; }
	void SetShadowDepth(float depth) { shadowDepth = depth; }

protected:
	MFVector textColour;

	MFString text;
	MFString font;

	MFFont *pFont;
	MFFontJustify textJustification;

	float textHeight;
	float shadowDepth;

	bool bAutoTextHeight;
	bool bOwnFont;
};

class HKWidgetRendererLabel : public HKWidgetRenderer
{
public:
	static HKWidgetRenderer *Create();
protected:
	virtual void Render(const HKWidget &widget, const MFMatrix &worldTransform);
};

#endif
