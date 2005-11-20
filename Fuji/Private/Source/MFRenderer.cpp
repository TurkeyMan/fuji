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
	CALLSTACK;

	memset(gRenderStates, 0, sizeof(gRenderStates));
	memset(gCurrentRenderStates, -1, sizeof(gCurrentRenderStates));

	for(int a=0; a<MFMT_Max; a++)
	{
		gTransformationMatrices[a] = MFMatrix::identity;
	}
}

void MFRenderer_DeinitModule()
{
	CALLSTACK;

}

int MFRenderer_Begin()
{
	CALLSTACK;

	return pCurrentMaterial->pType->materialCallbacks.pBegin(pCurrentMaterial);
}

const MFMatrix& MFRenderer_GetMatrix(MFMatrixType type)
{
	CALLSTACK;

	return gTransformationMatrices[type];
}

void MFRenderer_SetMatrix(MFMatrixType type, const MFMatrix &matrix)
{
	CALLSTACK;

	gTransformationMatrices[type] = matrix;
}

uint32 MFRenderer_SetRenderStateOverride(uint32 renderState, uint32 value)
{
	CALLSTACK;

	uint32 t = gRenderStates[renderState];
	gRenderStates[renderState] = value;

	return t;
}

uint32 MFRenderer_GetRenderStateOverride(uint32 renderState)
{
	CALLSTACK;

	return gRenderStates[renderState];
}
