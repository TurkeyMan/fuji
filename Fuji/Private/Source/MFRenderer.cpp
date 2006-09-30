#include "Fuji.h"
#include "MFRenderer_Internal.h"
#include "MFMaterial_Internal.h"
#include "MFView.h"

uint32 gRenderStates[MFRS_Max];
uint32 gCurrentRenderStates[MFRS_Max];

extern MFMaterial *pCurrentMaterial;

MFMatrix gTransformationMatrices[MFMT_Max];

void MFRenderer_InitModule()
{
	MFCALLSTACK;

	MFZeroMemory(gRenderStates, sizeof(gRenderStates));
	MFSetMemory(gCurrentRenderStates, -1, sizeof(gCurrentRenderStates));

	for(int a=0; a<MFMT_Max; a++)
	{
		gTransformationMatrices[a] = MFMatrix::identity;
	}
}

void MFRenderer_DeinitModule()
{
	MFCALLSTACK;

}

int MFRenderer_Begin()
{
	MFCALLSTACK;

	return pCurrentMaterial->pType->materialCallbacks.pBegin(pCurrentMaterial);
}

const MFMatrix& MFRenderer_GetMatrix(MFMatrixType type)
{
	MFCALLSTACK;

	return gTransformationMatrices[type];
}

void MFRenderer_SetMatrix(MFMatrixType type, const MFMatrix &matrix)
{
	MFCALLSTACK;

	gTransformationMatrices[type] = matrix;
}

uint32 MFRenderer_SetRenderStateOverride(uint32 renderState, uint32 value)
{
	MFCALLSTACK;

	uint32 t = gRenderStates[renderState];
	gRenderStates[renderState] = value;

	return t;
}

uint32 MFRenderer_GetRenderStateOverride(uint32 renderState)
{
	MFCALLSTACK;

	return gRenderStates[renderState];
}
