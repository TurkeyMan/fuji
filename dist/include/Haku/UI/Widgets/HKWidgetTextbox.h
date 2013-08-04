#pragma once
#if !defined(_HKWIDGET_TEXTBOX_H)
#define _HKWIDGET_TEXTBOX_H

#include "Haku/UI/HKStringEntryLogic.h"
#include "Haku/UI/HKWidget.h"
#include "Haku/UI/HKWidgetRenderer.h"

#include "Fuji/MFFont.h"

class HKWidgetTextbox : public HKWidget
{
	friend class HKWidgetRendererTextbox;
public:
	static HKWidget *Create(HKWidgetType *pType);
	static const char *TypeName() { return "Textbox"; }

	HKWidgetTextbox(HKWidgetType *pType);
	virtual ~HKWidgetTextbox();

	virtual void SetProperty(const char *pProperty, const char *pValue);
	virtual MFString GetProperty(const char *pProperty);

	MFString GetString() const { return stringLogic.GetString(); }
	MFString GetRenderString() const { return stringLogic.GetRenderString(); }
	MFString GetFontName() const { return font; }
	MFFont *GetFont() const { return pFont; }
	const MFVector &GetTextColour() { return textColour; }
	float GetTextHeight() const { return textHeight; }
	int GetCursorPos() const { return stringLogic.GetCursorPos(); }
	void GetSelection(int *pStart, int *pEnd) const { stringLogic.GetSelection(pStart, pEnd); }

	bool IsEmpty() const { return stringLogic.GetString().IsEmpty(); }
	bool HasFocus() const;

	void LoadFont(MFString font);

	void SetString(MFString string);
	void SetFont(MFFont *pFont);
	void SetTextColour(const MFVector &colour) { textColour = colour; }
	void SetTextHeight(float height);

	void SetType(HKStringEntryLogic::StringType type) { stringLogic.SetType(type); }

	HKWidgetEvent OnChanged;

protected:
	virtual void Update();
	virtual bool InputEvent(HKInputManager &manager, const HKInputManager::EventInfo &ev);
	void StringChangeCallback(MFString string);
	void UpdateCursorPos(float x, bool bUpdateSelection);

	MFVector textColour;

	HKStringEntryLogic stringLogic;

	MFString font;
	MFFont *pFont;

	HKInputSource *pFocusKeyboard;

	float textHeight;
	float padding;

	bool bAutoTextHeight;
	bool bOwnFont;

	static const EnumKeypair sTypeStrings[];
	static float blinkTime;
};

class HKWidgetRendererTextbox : public HKWidgetRenderer
{
public:
	static HKWidgetRenderer *Create(HKWidgetRendererType *pType);
protected:
	virtual void Render(const HKWidget &widget, const MFMatrix &worldTransform);
};

#endif
