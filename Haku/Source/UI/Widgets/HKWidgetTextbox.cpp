#include "Haku.h"
#include "UI/HKUI.h"
#include "UI/Widgets/HKWidgetTextbox.h"

#include "MFFont.h"

const EnumKeypair HKWidgetTextbox::sTypeStrings[] =
{
	{ "regular", HKStringEntryLogic::ST_Regular },
	{ "multiline", HKStringEntryLogic::ST_MultiLine },
	{ "numeric", HKStringEntryLogic::ST_Numeric },
	{ "password", HKStringEntryLogic::ST_Password }
};

float HKWidgetTextbox::blinkTime = 0.4f;

HKWidget *HKWidgetTextbox::Create()
{
	return new HKWidgetTextbox();
}


HKWidgetTextbox::HKWidgetTextbox()
{
	stringLogic.SetChangeCallback(fastdelegate::MakeDelegate(this, &HKWidgetTextbox::StringChangeCallback));

	pTypeName = "HKWidgetTextbox";

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
		MFFont_Destroy(pFont);
}

void HKWidgetTextbox::Update()
{
	HKWidget::Update();

	if(HasFocus())
		stringLogic.Update();
}

bool HKWidgetTextbox::InputEvent(HKInputManager &manager, HKInputManager::EventInfo &ev)
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
			MFFont_Destroy(pFont);
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

void HKWidgetTextbox::StringChangeCallback(const char *pString)
{
	blinkTime = 0.4f;

	if(!OnChanged.IsEmpty())
	{
		HKWidgetTextEvent ev(this, pString);
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


#include "MFRenderer.h"
#include "MFTexture.h"
#include "MFPrimitive.h"
#include "MFSystem.h"

HKWidgetRenderer *HKWidgetRendererTextbox::Create()
{
	return new HKWidgetRendererTextbox();
}

void HKWidgetRendererTextbox::Render(const HKWidget &widget, const MFMatrix &worldTransform)
{
	HKWidgetTextbox &tb = (HKWidgetTextbox&)widget;

	HKWidgetRenderer::Render(widget, worldTransform);

	const MFVector &size = widget.GetSize();
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

	// draw the frame and selection
	MFPrimitive(PT_TriStrip | PT_Prelit | PT_Untextured);
	MFSetMatrix(worldTransform);

	MFBegin(bDrawSelection ? 16 : 10);

	MFSetColour(MFVector::white);
	MFSetPosition(0.f, 0.f, 0);
	MFSetPosition(size.x, 0.f, 0);
	MFSetPosition(0.f, size.y, 0);
	MFSetPosition(size.x, size.y, 0);

	// degen
	MFSetPosition(0.f+size.x, 0.f+size.y, 0);
	MFSetPosition(padding, padding, 0);

	MFSetColour(MFVector::black);
	MFSetPosition(padding, padding, 0);
	MFSetPosition(size.x-padding, padding, 0);
	MFSetPosition(padding, size.y-padding, 0);
	MFSetPosition(size.x-padding, size.y-padding, 0);

	if(bDrawSelection)
	{
		// draw selection (if selected)
		int selMin = MFMin(selectionStart, selectionEnd);
		int selMax = MFMax(selectionStart, selectionEnd);

		float selMinX = MFFont_GetStringWidth(pFont, pString, textHeight, 10000, selMin);
		float selMaxX = MFFont_GetStringWidth(pFont, pString, textHeight, 10000, selMax);

		// degen
		MFSetPosition(size.x-padding, size.y-padding, 0);
		MFSetPosition(padding+selMinX, padding, 0);

		if(bHasFocus)
			MFSetColour(0, 0, 0.6f, 1);
		else
			MFSetColour(0.4f, 0.4f, 0.4f, 1);
		MFSetPosition(padding+selMinX, padding, 0);
		MFSetPosition(padding+selMaxX, padding, 0);
		MFSetPosition(padding+selMinX, padding+textHeight, 0);
		MFSetPosition(padding+selMaxX, padding+textHeight, 0);
	}

	MFEnd();

	if(!text.IsEmpty())
	{
		// draw text
		const MFVector &textColour = tb.GetTextColour();
		MFFont_DrawText(pFont, padding, padding, textHeight, bEnabled ? textColour : MFVector::grey, pString, -1, worldTransform);
	}

	if(bHasFocus)
	{
		// blink cursor
		tb.blinkTime -= MFSystem_TimeDelta();
		if(tb.blinkTime < -0.4f) tb.blinkTime += 0.8f;
		bool bCursor = tb.blinkTime > 0.0f;

		// draw cursor
		if(bCursor)
		{
			float cursorX = MFFont_GetStringWidth(pFont, pString, textHeight, 10000, cursorPos);

			// render cursor
			MFPrimitive(PT_TriStrip | PT_Prelit | PT_Untextured);
			MFSetMatrix(worldTransform);

			MFBegin(4);

			MFSetColour(MFVector::white);
			MFSetPosition(padding+cursorX, padding+1.f, 0);
			MFSetPosition(padding+2.f+cursorX, padding+1.f, 0);
			MFSetPosition(padding+cursorX, padding+textHeight-1.f, 0);
			MFSetPosition(padding+2.f+cursorX, padding+textHeight-1.f, 0);

			MFEnd();
		}
	}
}
