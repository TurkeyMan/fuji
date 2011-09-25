#include "Haku.h"
#include "UI/HKWidget.h"
#include "UI/HKWidgetRenderer.h"

#include "MFMaterial.h"
#include "../../Private/Source/Materials/MFMat_Standard.h"
#include "MFTexture.h"
#include "MFRenderer.h"
#include "MFPrimitive.h"

HKWidgetRenderer *HKWidgetRenderer::Create()
{
	return new HKWidgetRenderer;
}

HKWidgetRenderer::HKWidgetRenderer()
{
	bgColour = MFVector::zero;
	pBackground = NULL;
}

HKWidgetRenderer::~HKWidgetRenderer()
{
	if(pBackground)
		MFMaterial_Destroy(pBackground);
}

void HKWidgetRenderer::Render(const HKWidget &widget, const MFMatrix &worldTransform)
{
	if(pBackground)
	{
		const MFVector &size = widget.GetSize();

		MFMaterial_SetMaterial(pBackground);

		float offset = 0;
		float tc = MFRenderer_GetTexelCenterOffset();
		if(tc > 0.f)
		{
			int texW, texH;
			MFTexture *pTex = MFMaterial_GetParameterT(pBackground, MFMatStandard_DifuseMap, 0);
			MFTexture_GetTextureDimensions(pTex, &texW, &texH);

			if(size.x == texW && size.y == texH)
				offset = tc;
		}

		MFPrimitive_DrawQuad(-offset, -offset, size.x, size.y, widget.GetColour(), 0, 0, 1, 1, worldTransform);
	}
	else if(bgColour.w > 0.f)
	{
		const MFVector &size = widget.GetSize();

		MFPrimitive_DrawUntexturedQuad(0, 0, size.x, size.y, bgColour, worldTransform);
	}
}

void HKWidgetRenderer::SetProperty(const char *pProperty, const char *pValue)
{
	if(!MFString_CaseCmp(pProperty, "background_image"))
	{
		if(pBackground)
			MFMaterial_Destroy(pBackground);
		pBackground = MFMaterial_Create(pValue);
	}
	else if(!MFString_CaseCmp(pProperty, "background_colour"))
	{
		bgColour = HKWidget_GetVectorFromString(pValue);
		if(bgColour.w == 0.f)
			bgColour.w = 1.f;
	}
}

MFString HKWidgetRenderer::GetProperty(const char *pProperty)
{
	return NULL;
}
