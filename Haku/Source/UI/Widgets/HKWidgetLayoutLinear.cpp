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
	int numChildren = GetNumChildren();
/*
	// fitting it to the chuldren seems complicated... :/
	if(fitFlags != 0)
	{
		// fit to largest child in each dimension
		MFVector fit = MFVector::zero;
		for(int a=0; a<numChildren; ++a)
		{
			HKWidget *pWidget = GetChild(a);
			const MFVector &cMargin = GetChildMargin(a);
			const MFVector &cSize = pWidget->GetSize();

			fit.x = MFMax(fit.x, cSize.x + cMargin.x + cMargin.z + margin.x + margin.z);
			fit.y = MFMax(fit.y, cSize.y + cMargin.y + cMargin.w + margin.y + margin.w);
		}

		// resize the layout
		MFVector newSize = size;
		if(fitFlags & FitContentHorizontal)
			newSize.x = fit.x;
		if(fitFlags & FitContentVertical)
			newSize.y = fit.y;
		SetSize(newSize);
	}
*/
	MFVector cPos = MakeVector(margin.x, margin.y);
	MFVector cSize = MakeVector(size.x - (margin.x + margin.z), size.y - (margin.y + margin.w));

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
			cPos.x += cSize.x + cMargin.x + cMargin.z;

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
				pWidget->SetSize(MakeVector(size.x, tSize.y));
				break;
			}
		}
		else
		{
			cPos.y += cSize.y + cMargin.y + cMargin.w;

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
				pWidget->SetSize(MakeVector(tSize.x, size.y));
				break;
			}
		}
	}
}
