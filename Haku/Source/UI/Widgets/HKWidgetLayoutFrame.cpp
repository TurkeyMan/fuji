#include "Haku.h"
#include "UI/HKUI.h"
#include "UI/Widgets/HKWidgetLayoutFrame.h"

HKWidget *HKWidgetLayoutFrame::Create()
{
	return new HKWidgetLayoutFrame();
}


HKWidgetLayoutFrame::HKWidgetLayoutFrame()
{
	pTypeName = "HKWidgetLayoutFrame";
}

HKWidgetLayoutFrame::~HKWidgetLayoutFrame()
{
}

void HKWidgetLayoutFrame::ArrangeChildren()
{
	int numChildren = GetNumChildren();

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

	MFVector cPos = MakeVector(margin.x, margin.y);
	MFVector cSize = MakeVector(size.x - (margin.x + margin.z), size.y - (margin.y + margin.w));

	for(int a=0; a<numChildren; ++a)
	{
		HKWidget *pWidget = GetChild(a);

		const MFVector &cMargin = GetChildMargin(a);
		const MFVector &size = pWidget->GetSize();
		MFVector tPos = cPos + MakeVector(cMargin.x, cMargin.y);
		MFVector tSize = cSize - MakeVector(cMargin.x + cMargin.z, cMargin.y + cMargin.w);

		switch(GetChildJustification(a))
		{
		case None:
		case TopLeft:
			pWidget->SetPosition(tPos);
			break;
		case TopCenter:
			pWidget->SetPosition(tPos + MakeVector((tSize.x - size.x) * 0.5f, 0));
			break;
		case TopRight:
			pWidget->SetPosition(tPos + MakeVector(tSize.x - size.x, 0));
			break;
		case TopFill:
			pWidget->SetPosition(tPos);
			pWidget->SetSize(MakeVector(tSize.x, size.y));
			break;
		case CenterLeft:
			pWidget->SetPosition(tPos + MakeVector(0, (tSize.y - size.y) * 0.5f));
			break;
		case Center:
			pWidget->SetPosition(tPos + MakeVector((tSize.x - size.x) * 0.5f, (tSize.y - size.y) * 0.5f));
			break;
		case CenterRight:
			pWidget->SetPosition(tPos + MakeVector(tSize.x - size.x, (tSize.y - size.y) * 0.5f));
			break;
		case CenterFill:
			pWidget->SetPosition(tPos + MakeVector(0, (tSize.y - size.y) * 0.5f));
			pWidget->SetSize(MakeVector(tSize.x, size.y));
			break;
		case BottomLeft:
			pWidget->SetPosition(tPos + MakeVector(0, tSize.y - size.y));
			break;
		case BottomCenter:
			pWidget->SetPosition(tPos + MakeVector((tSize.x - size.x) * 0.5f, tSize.y - size.y));
			break;
		case BottomRight:
			pWidget->SetPosition(tPos + MakeVector(tSize.x - size.x, tSize.y - size.y));
			break;
		case BottomFill:
			pWidget->SetPosition(tPos + MakeVector(0, tSize.y - size.y));
			pWidget->SetSize(MakeVector(tSize.x, size.y));
			break;
		case FillLeft:
			pWidget->SetPosition(tPos);
			pWidget->SetSize(MakeVector(size.x, tSize.y));
			break;
		case FillCenter:
			pWidget->SetPosition(tPos + MakeVector((tSize.x - size.x) * 0.5f, 0));
			pWidget->SetSize(MakeVector(size.x, tSize.y));
			break;
		case FillRight:
			pWidget->SetPosition(tPos + MakeVector(tSize.x - size.x, 0));
			pWidget->SetSize(MakeVector(size.x, tSize.y));
			break;
		case Fill:
			pWidget->SetPosition(tPos);
			pWidget->SetSize(tSize);
			break;
		}
	}
}
