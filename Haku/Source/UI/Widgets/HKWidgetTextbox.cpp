#include "Haku.h"
#include "UI/HKUI.h"
#include "UI/Widgets/HKWidgetTextbox.h"

#include "Fuji/MFFont.h"

const EnumKeypair HKWidgetTextbox::sTypeStrings[] =
{
	{ "regular", HKStringEntryLogic::ST_Regular },
	{ "multiline", HKStringEntryLogic::ST_MultiLine },
	{ "numeric", HKStringEntryLogic::ST_Numeric },
	{ "password", HKStringEntryLogic::ST_Password }
};

float HKWidgetTextbox::blinkTime = 0.4f;

HKWidget *HKWidgetTextbox::Create(HKWidgetType *pType)
{
	return new HKWidgetTextbox(pType);
}


HKWidgetTextbox::HKWidgetTextbox(HKWidgetType *pType)
: HKWidget(pType)
{
	stringLogic.SetChangeCallback(fastdelegate::MakeDelegate(this, &HKWidgetTextbox::StringChangeCallback));

	pFocusKeyboard = NULL;

	textColour = MFVector::white;

	pFont = MFFont_GetDebugFont();
	bOwnFont = false;

	bAutoTextHeight = true;
	textHeight = MFFont_GetFontHeight(pFont);

	padding = 2.f;

	bClickable = true;

	if(bAutoHeight)
		UpdateHeight(textHeight + padding*2.f);
}

HKWidgetTextbox::~HKWidgetTextbox()
{
	if(bOwnFont)
		MFFont_Release(pFont);
}

void HKWidgetTextbox::Update()
{
	HKWidget::Update();

	if(HasFocus())
		stringLogic.Update();
}

bool HKWidgetTextbox::InputEvent(HKInputManager &manager, const HKInputManager::EventInfo &ev)
{
	// try and handle the input event in some standard ways...
	switch(ev.ev)
	{
		case HKInputManager::IE_Down:
		{
			// set the cursor pos
			bool bUpdateSelection = MFInput_Read(Key_LShift, IDD_Keyboard) || MFInput_Read(Key_RShift, IDD_Keyboard);
			UpdateCursorPos(ev.down.x, bUpdateSelection);

			// allow drag selection
			GetUI().SetFocus(ev.pSource, this);

			// also claim keyboard focus...
			pFocusKeyboard = manager.FindSource(IDD_Keyboard, ev.pSource->deviceID);
			if(pFocusKeyboard)
				GetUI().SetFocus(pFocusKeyboard, this);

			blinkTime = 0.4f;
			break;
		}
		case HKInputManager::IE_Up:
		{
			GetUI().SetFocus(ev.pSource, NULL);
		}
		case HKInputManager::IE_Drag:
		{
			// drag text selection
			UpdateCursorPos(ev.drag.x, true);
			blinkTime = 0.4f;
			break;
		}
		default:
			break;
	}

	return HKWidget::InputEvent(manager, ev);
}

void HKWidgetTextbox::SetProperty(const char *pProperty, const char *pValue)
{
	if(!MFString_CaseCmp(pProperty, "text"))
		SetString(pValue);
	else if(!MFString_CaseCmp(pProperty, "text_colour"))
		SetTextColour(HKWidget_GetColourFromString(pValue));
	else if(!MFString_CaseCmp(pProperty, "text_height"))
		SetTextHeight(MFString_AsciiToFloat(pValue));
	else if(!MFString_CaseCmp(pProperty, "text_font"))
	{
		if(bOwnFont)
			MFFont_Release(pFont);
		pFont = MFFont_Create(pValue);
		bOwnFont = true;

		if(bAutoTextHeight)
			textHeight = MFFont_GetFontHeight(pFont);

		if(bAutoHeight)
		{
			MFVector newSize = GetSize();
			newSize.y = textHeight + padding*2.f;
			Resize(newSize);
		}
	}
	else if(!MFString_CaseCmp(pProperty, "type"))
		SetType((HKStringEntryLogic::StringType)HKWidget_GetEnumValue(pValue, sTypeStrings));
	else if(!MFString_CaseCmp(pProperty, "onChanged"))
		HKWidget_BindWidgetEvent(OnChanged, pValue);
	else
		HKWidget::SetProperty(pProperty, pValue);
}

MFString HKWidgetTextbox::GetProperty(const char *pProperty)
{
	if(!MFString_CaseCmp(pProperty, "text"))
		return GetString();
	else if(!MFString_CaseCmp(pProperty, "text_font"))
	{
		if(bOwnFont)
			return font;
		else
//			return MFFont_GetFontName(pFont);
			return NULL;
	}
	return HKWidget::GetProperty(pProperty);
}

bool HKWidgetTextbox::HasFocus() const
{
	return pFocusKeyboard && GetUI().GetFocus(pFocusKeyboard) == this;
}

void HKWidgetTextbox::SetString(MFString string)
{
	stringLogic.SetString(string);
}

void HKWidgetTextbox::SetTextHeight(float height)
{
	this->textHeight = height;
	bAutoTextHeight = false;

	if(bAutoHeight)
	{
		MFVector newSize = GetSize();
		newSize.y = textHeight + padding*2.f;
		Resize(newSize);
	}
}

void HKWidgetTextbox::StringChangeCallback(MFString string)
{
	blinkTime = 0.4f;

	if(!OnChanged.IsEmpty())
	{
		HKWidgetTextEvent ev(this, string);
		OnChanged(*this, ev);
	}
}

void HKWidgetTextbox::UpdateCursorPos(float x, bool bUpdateSelection)
{
	MFString text = stringLogic.GetRenderString();
	const char *pString = text.CStr();

	float magnitude = 10000000.f, downPos = x - 2;
	int offset = 0, len = text.NumChars();
	for(int a=0; a<=len; ++a)
	{
		float x = MFFont_GetStringWidth(pFont, pString, textHeight, 100000, a);
		float m = MFAbs(x - downPos);
		if(m < magnitude)
		{
			magnitude = m;
			offset = a;
		}
		else
			break;
	}

	stringLogic.SetCursorPos(offset, bUpdateSelection);
}


#include "Fuji/MFRenderer.h"
#include "Fuji/MFTexture.h"
#include "Fuji/MFPrimitive.h"
#include "Fuji/MFSystem.h"

HKWidgetRenderer *HKWidgetRendererTextbox::Create(HKWidgetRendererType *pType)
{
	return new HKWidgetRendererTextbox();
}

void HKWidgetRendererTextbox::Render(const HKWidget &widget, const MFMatrix &worldTransform)
{
	HKWidgetTextbox &tb = (HKWidgetTextbox&)widget;

	HKWidgetRenderer::Render(widget, worldTransform);

	float textHeight = tb.GetTextHeight();
	float padding = tb.padding;

	MFString text = tb.GetRenderString();
	MFFont *pFont = tb.GetFont();
	const char *pString = text.CStr();

	int selectionStart, selectionEnd;
	int cursorPos = tb.GetCursorPos();
	tb.GetSelection(&selectionStart, &selectionEnd);

	bool bDrawSelection = selectionStart != selectionEnd;
	bool bEnabled = widget.IsEnabled();
	bool bHasFocus = tb.HasFocus();

	if(bDrawSelection)
	{
		// draw selection (if selected)
		int selMin = MFMin(selectionStart, selectionEnd);
		int selMax = MFMax(selectionStart, selectionEnd);

		float selMinX = MFFont_GetStringWidth(pFont, pString, textHeight, 10000, selMin);
		float selMaxX = MFFont_GetStringWidth(pFont, pString, textHeight, 10000, selMax);

		MFVector selectionColour = bHasFocus ? MakeVector(0, 0, 1, 0.6f) : MakeVector(1, 1, 1, 0.4f);
		MFPrimitive_DrawUntexturedQuad(padding+selMinX, padding, selMaxX-selMinX, textHeight, selectionColour, worldTransform);
	}

	if(!text.IsEmpty())
	{
		// draw text
		const MFVector &textColour = tb.GetTextColour();
		MFFont_DrawText2(pFont, padding, padding, textHeight, bEnabled ? textColour : MFVector::grey, pString, -1, worldTransform);
	}

	if(bHasFocus)
	{
		// blink cursor
		tb.blinkTime -= MFTimeDelta();
		if(tb.blinkTime < -0.4f) tb.blinkTime += 0.8f;
		bool bCursor = tb.blinkTime > 0.0f;

		// draw cursor
		if(bCursor)
		{
			float cursorX = MFFont_GetStringWidth(pFont, pString, textHeight, 10000, cursorPos);

			// render cursor
			MFPrimitive_DrawUntexturedQuad(padding+cursorX, padding+1.f, 2, textHeight-2.f, MFVector::white, worldTransform);
		}
	}
}
