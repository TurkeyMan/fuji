#include "Haku.h"
#include "UI/HKWidget.h"
#include "UI/HKWidgetRenderer.h"

#include "MFMaterial.h"
#include "../../Private/Source/Materials/MFMat_Standard.h"
#include "MFTexture.h"
#include "MFRenderer.h"
#include "MFPrimitive.h"

HKWidgetRenderer *HKWidgetRenderer::Create(HKWidgetRendererType *pType)
{
	return new HKWidgetRenderer;
}

HKWidgetRenderer::HKWidgetRenderer()
{
	padding = MFVector::zero;
	colour = MFVector::zero;
	border = MFVector::zero;
	borderColour = MFVector::black;
	pImage = NULL;
	imageAlignment = HKWidget::Center;
	margin9Cell = 0.f;
}

HKWidgetRenderer::~HKWidgetRenderer()
{
	if(pImage)
		MFMaterial_Destroy(pImage);
}

void HKWidgetRenderer::Render(const HKWidget &widget, const MFMatrix &worldTransform)
{
	MFVector widgetColour = widget.GetColour();
	MFVector size = widget.GetSize();
	size.x -= padding.x + padding.z;
	size.y -= padding.y + padding.w;

	if(colour.w > 0.f)
	{
		float borderWidth = border.x + border.z;
		float borderHeight = border.y + border.w;
		MFPrimitive_DrawUntexturedQuad(padding.x + border.x, padding.y + border.y, size.x - borderWidth, size.y - borderHeight, colour*widgetColour, worldTransform);
	}

	if(border.x > 0.f) // left
		MFPrimitive_DrawUntexturedQuad(padding.x, padding.y, border.x, size.y, borderColour*widgetColour, worldTransform);
	if(border.y > 0.f) // top
		MFPrimitive_DrawUntexturedQuad(padding.x, padding.y, size.x, border.y, borderColour*widgetColour, worldTransform);
	if(border.z > 0.f) // right
		MFPrimitive_DrawUntexturedQuad(size.x - border.z + padding.x, padding.y, border.z, size.y, borderColour*widgetColour, worldTransform);
	if(border.w > 0.f) // bottom
		MFPrimitive_DrawUntexturedQuad(padding.x, padding.y + size.y - border.w, size.x, border.w, borderColour*widgetColour, worldTransform);

	if(pImage)
	{
		if(margin9Cell > 0.f)
		{
			// 9 cell stuff...
		}
		else
		{
			// draw the background image centered in the box
			MFMaterial_SetMaterial(pImage);

			float offset = 0;
			float tc = MFRenderer_GetTexelCenterOffset();
			if(tc > 0.f)
			{
				if(size.x == texWidth && size.y == texHeight)
					offset = tc;
			}

			MFPrimitive_DrawQuad(padding.x - offset, padding.y - offset, size.x, size.y, widgetColour, 0, 0, 1, 1, worldTransform);
		}
	}
}

bool HKWidgetRenderer::SetProperty(const char *pProperty, const char *pValue)
{
	if(!MFString_CaseCmp(pProperty, "background_image"))
	{
		if(pImage)
			MFMaterial_Destroy(pImage);
		pImage = MFMaterial_Create(pValue);
		if(pImage)
		{
			int texW, texH;
			MFTexture *pTex = MFMaterial_GetParameterT(pImage, MFMatStandard_DifuseMap, 0);
			MFTexture_GetTextureDimensions(pTex, &texW, &texH);

			texWidth = (float)texW;
			texHeight = (float)texH;
		}
		return true;
	}
	else if(!MFString_CaseCmp(pProperty, "background_align"))
	{
		imageAlignment = (HKWidget::Justification)HKWidget_GetEnumValue(pValue, HKWidget::sJustifyKeys);
		return true;
	}
	else if(!MFString_CaseCmp(pProperty, "background_colour"))
	{
		colour = HKWidget_GetColourFromString(pValue);
		return true;
	}
	else if(!MFString_CaseCmp(pProperty, "background_padding"))
	{
		padding = HKWidget_GetVectorFromString(pValue);
		return true;
	}
	else if(!MFString_CaseCmp(pProperty, "background_9-cell-margin"))
	{
		margin9Cell = MFString_AsciiToFloat(pValue);
		return true;
	}
	else if(!MFString_CaseCmp(pProperty, "border_width"))
	{
		border = HKWidget_GetVectorFromString(pValue);
		return true;
	}
	else if(!MFString_CaseCmp(pProperty, "border_colour"))
	{
		borderColour = HKWidget_GetColourFromString(pValue);
		return true;
	}
	return false;
}

MFString HKWidgetRenderer::GetProperty(const char *pProperty)
{
	return NULL;
}
