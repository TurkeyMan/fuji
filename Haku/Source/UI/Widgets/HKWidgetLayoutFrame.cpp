#include "Haku.h"
#include "UI/HKUI.h"
#include "UI/Widgets/HKWidgetLayoutFrame.h"

HKWidget *HKWidgetLayoutFrame::Create(HKWidgetType *pType)
{
	return new HKWidgetLayoutFrame(pType);
}


HKWidgetLayoutFrame::HKWidgetLayoutFrame(HKWidgetType *pType)
: HKWidgetLayout(pType)
{
}

HKWidgetLayoutFrame::~HKWidgetLayoutFrame()
{
}

void HKWidgetLayoutFrame::ArrangeChildren()
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

	bool bFitWidth = bAutoWidth && (layoutJustification & 3) != 3; // fitFlags & FitContentHorizontal
	bool bFitHeight = bAutoHeight && (layoutJustification >> 2) != 3; // fitFlags & FitContentVertical

	if(bFitWidth || bFitHeight)
	{
		// fit to largest child in each dimension
		MFVector fit = MFVector::zero;
		for(int a=0; a<numChildren; ++a)
		{
			HKWidget *pWidget = GetChild(a);
			const MFVector &cSize = pWidget->GetSizeWithMargin();

			fit.x = MFMax(fit.x, cSize.x + padding.x + padding.z);
			fit.y = MFMax(fit.y, cSize.y + padding.y + padding.w);
		}

		// resize the layout
		MFVector newSize = GetSize();
		if(bFitWidth)
			newSize.x = fit.x;
		if(bFitHeight)
			newSize.y = fit.y;
		Resize(newSize);
	}

	MFVector cPos = MakeVector(padding.x, padding.y);
	MFVector cSize = MakeVector(size.x - (padding.x + padding.z), size.y - (padding.y + padding.w));

	for(int a=0; a<numChildren; ++a)
	{
		HKWidget *pWidget = GetChild(a);

		const MFVector &cMargin = pWidget->GetLayoutMargin();
		const MFVector &size = pWidget->GetSize();
		MFVector tPos = cPos + MakeVector(cMargin.x, cMargin.y);
		MFVector tSize = cSize - MakeVector(cMargin.x + cMargin.z, cMargin.y + cMargin.w);

		switch(pWidget->GetLayoutJustification())
		{
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
			ResizeChild(pWidget, MakeVector(tSize.x, size.y));
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
			ResizeChild(pWidget, MakeVector(tSize.x, size.y));
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
			ResizeChild(pWidget, MakeVector(tSize.x, size.y));
			break;
		case FillLeft:
			pWidget->SetPosition(tPos);
			ResizeChild(pWidget, MakeVector(size.x, tSize.y));
			break;
		case FillCenter:
			pWidget->SetPosition(tPos + MakeVector((tSize.x - size.x) * 0.5f, 0));
			ResizeChild(pWidget, MakeVector(size.x, tSize.y));
			break;
		case FillRight:
			pWidget->SetPosition(tPos + MakeVector(tSize.x - size.x, 0));
			ResizeChild(pWidget, MakeVector(size.x, tSize.y));
			break;
		case Fill:
			pWidget->SetPosition(tPos);
			ResizeChild(pWidget, tSize);
			break;
		case None:
			// this widget has absolute coordinates..
		default:
			break;
		}
	}
}
