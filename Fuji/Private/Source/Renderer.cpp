#include "Common.h"
#include "Renderer.h"
#include "Material.h"
#include "Model.h"
#include "Animation.h"
#include "View.h"

uint32 renderStates[RS_Max];
uint32 currentRenderStates[RS_Max];

Material *pCurrentMaterial = NULL;
uint32 renderSource = NULL;
View *pCurrentView = NULL;

void Renderer_InitModule()
{
	memset(renderStates, 0, sizeof(renderStates));
	memset(currentRenderStates, -1, sizeof(currentRenderStates));
}

void Renderer_DeinitModule()
{

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
