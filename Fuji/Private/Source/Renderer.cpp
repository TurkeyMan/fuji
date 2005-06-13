#include "Common.h"
#include "Renderer_Internal.h"
#include "MFMaterial_Internal.h"
#include "View.h"

uint32 renderStates[RS_Max];
uint32 currentRenderStates[RS_Max];

extern MFMaterial *pCurrentMaterial;

void Renderer_InitModule()
{
	memset(renderStates, 0, sizeof(renderStates));
	memset(currentRenderStates, -1, sizeof(currentRenderStates));
}

void Renderer_DeinitModule()
{

}

int Renderer_Begin()
{
	return pCurrentMaterial->pType->materialCallbacks.pBegin(pCurrentMaterial);
}

uint32 SetRenderStateOverride(uint32 renderState, uint32 value)
{
	uint32 t = renderStates[renderState];
	renderStates[renderState] = value;

	return t;
}

uint32 GetRenderStateOverride(uint32 renderState)
{
	return renderStates[renderState];
}
