#include "Haku.h"
#include "UI/HKUI.h"
#include "UI/Widgets/HKWidgetLayoutLinear.h"

const EnumKeypair HKWidgetLayoutLinear::sOrientationKeys[] =
{
	{ "Horizontal", Horizontal },
	{ "Vertical", Vertical },
	{ NULL, 0 }
};

HKWidget *HKWidgetLayoutLinear::Create(HKWidgetType *pType)
{
	return new HKWidgetLayoutLinear(pType);
}


HKWidgetLayoutLinear::HKWidgetLayoutLinear(HKWidgetType *pType)
: HKWidgetLayout(pType)
{
	orientation = Horizontal;
}

HKWidgetLayoutLinear::~HKWidgetLayoutLinear()
{
}

void HKWidgetLayoutLinear::SetProperty(const char *pProperty, const char *pValue)
{
	if(!MFString_CaseCmp(pProperty, "orientation"))
		SetOrientation((Orientation)HKWidget_GetEnumValue(pValue, sOrientationKeys));
	else
		HKWidgetLayout::SetProperty(pProperty, pValue);
}

MFString HKWidgetLayoutLinear::GetProperty(const char *pProperty)
{
	if(!MFString_CaseCmp(pProperty, "orientation"))
		return HKWidget_GetEnumFromValue(GetOrientation(), sOrientationKeys);
	return HKWidgetLayout::GetProperty(pProperty);
}

void HKWidgetLayoutLinear::ArrangeChildren()
{
	bool bFitWidth = bAutoWidth && GetHAlign() != Align_Fill; // fitFlags & FitContentHorizontal
	bool bFitHeight = bAutoHeight && GetVAlign() != VAlign_Fill; // fitFlags & FitContentVertical

	// early out?
	int numChildren = GetNumChildren();
	if(numChildren == 0)
	{
		if(bFitWidth || bFitHeight)
		{
			// resize the layout
			MFVector newSize = GetSize();
			if(bFitWidth)
				newSize.x = padding.x + padding.z;
			if(bFitHeight)
				newSize.y = padding.y + padding.w;
			Resize(newSize);
		}
		return;
	}

	// calculate weight and fit
	float totalWeight = 0.f;
	MFVector fit = MakeVector(padding.x + padding.z, padding.y + padding.w);
	for(int a=0; a<numChildren; ++a)
	{
		HKWidget *pWidget = GetChild(a);
		if(pWidget->GetVisible() == Gone)
			continue;

		const MFVector &cSize = pWidget->GetSizeWithMargin();

		if(orientation == Horizontal)
		{
			if(pWidget->GetHAlign() == Align_Fill) // fill horizontally
				totalWeight += pWidget->GetLayoutWeight();
			else
				fit.x += cSize.x;

			fit.y = MFMax(fit.y, cSize.y + padding.y + padding.w);
		}
		else
		{
			if(pWidget->GetVAlign() == VAlign_Fill) // fill vertically
				totalWeight += pWidget->GetLayoutWeight();
			else
				fit.y += cSize.y;

			fit.x = MFMax(fit.x, cSize.x + padding.x + padding.z);
		}
	}

	if(bFitWidth || bFitHeight)
	{
		// resize the layout
		MFVector newSize = size;
		if(bFitWidth)
			newSize.x = fit.x;
		if(bFitHeight)
			newSize.y = fit.y;
		Resize(newSize);
	}

	MFVector pPos = MakeVector(padding.x, padding.y);
	MFVector pSize = MakeVector(size.x - (padding.x + padding.z), size.y - (padding.y + padding.w));

	MFVector slack = MFMax(size - fit, MFVector::zero);

	for(int a=0; a<numChildren; ++a)
	{
		HKWidget *pWidget = GetChild(a);
		if(pWidget->GetVisible() == Gone)
			continue;

		const MFVector &cMargin = pWidget->GetLayoutMargin();
		const MFVector &cSize = pWidget->GetSize();

		MFVector tPos = pPos + MakeVector(cMargin.x, cMargin.y);
		MFVector tSize = MFMax(pSize - MakeVector(cMargin.x + cMargin.z, cMargin.y + cMargin.w), MFVector::zero);

		Align align = pWidget->GetHAlign();
		VAlign valign = pWidget->GetVAlign();

		MFVector newSize = cSize;

		if(orientation == Horizontal)
		{
			if(align == Align_Fill) // fill horizontally
			{
				// this widget fills available empty space in the parent container
				newSize.x = slack.x * (pWidget->GetLayoutWeight() / totalWeight);
				pPos.x += newSize.x;
				newSize.x = MFMax(0.f, newSize.x - cMargin.x - cMargin.z);
			}
			else
			{
				pPos.x += cSize.x + cMargin.x + cMargin.z;
			}

			switch(valign)
			{
			case VAlign_None:
			case VAlign_Top:
				pWidget->SetPosition(tPos);
				break;
			case VAlign_Center:
				pWidget->SetPosition(tPos + MakeVector(0, MFMax(tSize.y - cSize.y, 0.f) * 0.5f));
				break;
			case VAlign_Bottom:
				pWidget->SetPosition(tPos + MakeVector(0, MFMax(tSize.y - cSize.y, 0.f)));
				break;
			case VAlign_Fill:
				pWidget->SetPosition(tPos);
				newSize.y = tSize.y;
				break;
			default:
				MFUNREACHABLE;
			}
		}
		else
		{
			if(valign == VAlign_Fill) // fill vertically
			{
				// this widget fills available empty space in the parent container
				newSize.y = slack.y * (pWidget->GetLayoutWeight() / totalWeight);
				pPos.y += newSize.y;
				newSize.y = MFMax(0.f, newSize.y - cMargin.y - cMargin.w);
			}
			else
			{
				pPos.y += cSize.y + cMargin.y + cMargin.w;
			}

			switch(align)
			{
				case Align_None:
				case Align_Left:
					pWidget->SetPosition(tPos);
					break;
				case Align_Center:
					pWidget->SetPosition(tPos + MakeVector(MFMax(tSize.x - cSize.x, 0.f) * 0.5f, 0));
					break;
				case Align_Right:
					pWidget->SetPosition(tPos + MakeVector(MFMax(tSize.x - cSize.x, 0.f), 0));
					break;
				case Align_Fill:
					pWidget->SetPosition(tPos);
					newSize.x = tSize.x;
					break;
				default:
					MFUNREACHABLE;
			}
		}

		ResizeChild(pWidget, newSize);
	}
}
