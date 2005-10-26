#include "Fuji.h"
#include "Renderer_Internal.h"
#include "MFMaterial_Internal.h"
#include "MFView.h"

uint32 renderStates[RS_Max];
uint32 currentRenderStates[RS_Max];

extern MFMaterial *pCurrentMaterial;

MFMatrix gTransformationMatrices[MFMT_Max];

void Renderer_InitModule()
{
	memset(renderStates, 0, sizeof(renderStates));
	memset(currentRenderStates, -1, sizeof(currentRenderStates));

	for(int a=0; a<MFMT_Max; a++)
	{
		gTransformationMatrices[a] = MFMatrix::identity;
	}
}

void Renderer_DeinitModule()
{

}

int Renderer_Begin()
{
	return pCurrentMaterial->pType->materialCallbacks.pBegin(pCurrentMaterial);
}

const MFMatrix& MKRenderer_GetMatrix(MatrixType type, MFMatrix *pMatrix)
{
	if(pMatrix)
		*pMatrix = gTransformationMatrices[type];

	return gTransformationMatrices[type];
}

void MKRenderer_SetMatrix(MatrixType type, const MFMatrix &matrix)
{
	gTransformationMatrices[type] = matrix;
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
