#include "Haku.h"
#include "UI/HKUI.h"
#include "UI/Widgets/HKWidgetLayoutLinear.h"

const EnumKeypair HKWidgetLayoutLinear::sOrientationKeys[] =
{
	{ "Horizontal", Horizontal },
	{ "Vertical", Vertical },
	{ NULL, 0 }
};

HKWidget *HKWidgetLayoutLinear::Create()
{
	return new HKWidgetLayoutLinear();
}


HKWidgetLayoutLinear::HKWidgetLayoutLinear()
{
	pTypeName = "HKWidgetLayoutLinear";

	orientation = Horizontal;
}

HKWidgetLayoutLinear::~HKWidgetLayoutLinear()
{
}

void HKWidgetLayoutLinear::SetProperty(const char *pProperty, const char *pValue)
{
	if(!MFString_CaseCmp(pProperty, "orientation"))
		SetOrientation((Orientation)HKWidget_GetEnumValue(pValue, sOrientationKeys));
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
	// early out?
	int numChildren = GetNumChildren();
	if(numChildren == 0)
	{
		if(bAutoWidth || bAutoHeight)
		{
			// resize the layout
			MFVector newSize = GetSize();
			if(bAutoWidth)
				newSize.x = padding.x + padding.z;
			if(bAutoHeight)
				newSize.y = padding.y + padding.w;
			Resize(newSize);
		}
		return;
	}

	// calculate weight and fix
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
			if((pWidget->GetLayoutJustification() & 3) == 3) // fill horizontally
				totalWeight += pWidget->GetLayoutWeight();
			else
				fit.x += cSize.x;

			fit.y = MFMax(fit.y, cSize.y + padding.y + padding.w);
		}
		else
		{
			if((pWidget->GetLayoutJustification() >> 2) & 3) // fill vertically
				totalWeight += pWidget->GetLayoutWeight();
			else
				fit.y += cSize.y;

			fit.x = MFMax(fit.x, cSize.x + padding.x + padding.z);
		}
	}

	bool bFitWidth = bAutoWidth && (layoutJustification & 3) != 3; // fitFlags & FitContentHorizontal
	bool bFitHeight = bAutoHeight && (layoutJustification >> 2) != 3; // fitFlags & FitContentVertical

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

		uint32 justify = pWidget->GetLayoutJustification();

		MFVector newSize = cSize;

		if(orientation == Horizontal)
		{
			if((justify & 3) == 3) // fill horizontally
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

			switch((justify >> 2) & 3)
			{
			case 0: // top
				pWidget->SetPosition(tPos);
				break;
			case 1: // center
				pWidget->SetPosition(tPos + MakeVector(0, MFMax(tSize.y - cSize.y, 0.f) * 0.5f));
				break;
			case 2: // bottom
				pWidget->SetPosition(tPos + MakeVector(0, MFMax(tSize.y - cSize.y, 0.f)));
				break;
			case 3: // fill
				pWidget->SetPosition(tPos);
				newSize.y = tSize.y;
				break;
			}
		}
		else
		{
			if((justify >> 2) & 3) // fill vertically
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

			switch(justify & 3)
			{
			case 0: // left
				pWidget->SetPosition(tPos);
				break;
			case 1: // center
				pWidget->SetPosition(tPos + MakeVector(MFMax(tSize.x - cSize.x, 0.f) * 0.5f, 0));
				break;
			case 2: // right
				pWidget->SetPosition(tPos + MakeVector(MFMax(tSize.x - cSize.x, 0.f), 0));
				break;
			case 3: // fill
				pWidget->SetPosition(tPos);
				newSize.x = tSize.x;
				break;
			}
		}

		ResizeChild(pWidget, newSize);
	}
}
