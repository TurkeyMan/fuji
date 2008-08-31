#include "Fuji.h"
#include "MFRenderer_Internal.h"
#include "MFMaterial_Internal.h"
#include "MFView.h"

uint32 gRenderStates[MFRS_Max];
uint32 gCurrentRenderStates[MFRS_Max];

extern MFMaterial *pCurrentMaterial;

MFMatrix gTransformationMatrices[MFMT_Max];

const MFMatrix *pAnimMats = NULL;
int gNumAnimMats = 0;

const uint16 *pCurrentBatch = NULL;
int gNumBonesInBatch = 0;

void MFRenderer_InitModule()
{
	MFCALLSTACK;

	MFZeroMemory(gRenderStates, sizeof(gRenderStates));
	MFMemSet(gCurrentRenderStates, -1, sizeof(gCurrentRenderStates));

	for(int a=0; a<MFMT_Max; a++)
	{
		gTransformationMatrices[a] = MFMatrix::identity;
	}

	MFRenderer_InitModulePlatformSpecific();
}

void MFRenderer_DeinitModule()
{
	MFCALLSTACK;

	MFRenderer_DeinitModulePlatformSpecific();
}

int MFRenderer_Begin()
{
	MFCALLSTACK;

	return pCurrentMaterial ? pCurrentMaterial->pType->materialCallbacks.pBegin(pCurrentMaterial) : 0;
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

void MFRenderer_SetMatrices(const MFMatrix *pMatrices, int numMatrices)
{
	pAnimMats = pMatrices;
	gNumAnimMats = numMatrices;
}

void MFRenderer_SetBatch(const uint16 *pBatch, int numBonesInBatch)
{
	pCurrentBatch = pBatch;
	gNumBonesInBatch = numBonesInBatch;
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
