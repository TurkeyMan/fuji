#include "Haku.h"
#include "UI/HKUI.h"
#include "UI/Widgets/HKWidgetLayoutLinear.h"

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

void HKWidgetLayoutLinear::ArrangeChildren()
{
	// early out?
	int numChildren = GetNumChildren();
	if(numChildren == 0)
		return;

	// calculate weight and fix
	float totalWeight = 0.f;
	MFVector fit = MakeVector(margin.x + margin.z, margin.y + margin.w);
	for(int a=0; a<numChildren; ++a)
	{
		HKWidget *pWidget = GetChild(a);
		const MFVector &cMargin = GetChildMargin(a);
		const MFVector &cSize = pWidget->GetSize();

		if(orientation == Horizontal)
		{
			if((GetChildJustification(a) & 3) == 3) // fill horizontally
				totalWeight += GetChildWeight(a);
			else
				fit.x += cSize.x + cMargin.x + cMargin.z;

			fit.y = MFMax(fit.y, cSize.y + cMargin.y + cMargin.w + margin.y + margin.w);
		}
		else
		{
			if((GetChildJustification(a) >> 2) & 3) // fill vertically
				totalWeight += GetChildWeight(a);
			else
				fit.y += cSize.y + cMargin.y + cMargin.w;

			fit.x = MFMax(fit.x, cSize.x + cMargin.x + cMargin.z + margin.x + margin.z);
		}
	}

	if(fitFlags != 0)
	{
		// resize the layout
		MFVector newSize = size;
		if(fitFlags & FitContentHorizontal)
			newSize.x = fit.x;
		if(fitFlags & FitContentVertical)
			newSize.y = fit.y;
		SetSize(newSize);
	}

	MFVector cPos = MakeVector(margin.x, margin.y);
	MFVector cSize = MakeVector(size.x - (margin.x + margin.z), size.y - (margin.y + margin.w));

	MFVector slack = MFMax(size - fit, MFVector::zero);

	for(int a=0; a<numChildren; ++a)
	{
		HKWidget *pWidget = GetChild(a);
		const MFVector &cMargin = GetChildMargin(a);
		const MFVector &cSize = pWidget->GetSize();

		MFVector tPos = cPos + MakeVector(cMargin.x, cMargin.y);
		MFVector tSize = cSize - MakeVector(cMargin.x + cMargin.z, cMargin.y + cMargin.w);

		uint32 justify = GetChildJustification(a);

		if(orientation == Horizontal)
		{
			MFVector newSize = cSize;

			if((GetChildJustification(a) & 3) == 3) // fill horizontally
			{
				// this widget fills available empty space in the parent container
				newSize.x = slack.x * (GetChildWeight(a) / totalWeight);
				cPos.x += newSize.x;
				newSize.x = MFMax(0.f, newSize.x - cMargin.x - cMargin.z);
			}
			else
			{
				cPos.x += cSize.x + cMargin.x + cMargin.z;
			}

			switch((justify >> 2) & 3)
			{
			case 0: // top
				pWidget->SetPosition(tPos);
				break;
			case 1: // center
				pWidget->SetPosition(tPos + MakeVector(0, (tSize.y - size.y) * 0.5f));
				break;
			case 2: // bottom
				pWidget->SetPosition(tPos + MakeVector(0, tSize.y - size.y));
				break;
			case 3: // fill
				pWidget->SetPosition(tPos);
				newSize.y = tSize.y;
				break;
			}

			pWidget->SetSize(newSize);
		}
		else
		{
			MFVector newSize = cSize;

			if((GetChildJustification(a) >> 2) & 3) // fill vertically
			{
				// this widget fills available empty space in the parent container
				newSize.y = slack.y * (GetChildWeight(a) / totalWeight);
				cPos.y += newSize.y;
				newSize.y = MFMax(0.f, newSize.y - cMargin.y - cMargin.w);
			}
			else
			{
				cPos.y += cSize.y + cMargin.y + cMargin.w;
			}

			switch(justify & 3)
			{
			case 0: // left
				pWidget->SetPosition(tPos);
				break;
			case 1: // center
				pWidget->SetPosition(tPos + MakeVector((tSize.x - size.x) * 0.5f, 0));
				break;
			case 2: // right
				pWidget->SetPosition(tPos + MakeVector(tSize.x - size.x, 0));
				break;
			case 3: // fill
				pWidget->SetPosition(tPos);
				newSize.x = tSize.x;
				break;
			}

			pWidget->SetSize(newSize);
		}
	}
}
