#include "Fuji.h"
#include "MFRenderer_Internal.h"
#include "MFMaterial_Internal.h"
#include "MFView.h"

uintp gRenderStates[MFRS_Max];
uintp gCurrentRenderStates[MFRS_Max];

extern MFMaterial *pCurrentMaterial;

MFMatrix gTransformationMatrices[MFMT_Max];

const MFMatrix *pAnimMats = NULL;
int gNumAnimMats = 0;

const uint16 *pCurrentBatch = NULL;
int gNumBonesInBatch = 0;

MFInitStatus MFRenderer_InitModule()
{
	MFCALLSTACK;

	MFZeroMemory(gRenderStates, sizeof(gRenderStates));
	MFMemSet(gCurrentRenderStates, -1, sizeof(gCurrentRenderStates));

	for(int a=0; a<MFMT_Max; a++)
	{
		gTransformationMatrices[a] = MFMatrix::identity;
	}

	MFRenderer_InitModulePlatformSpecific();

	return MFAIC_Succeeded;
}

void MFRenderer_DeinitModule()
{
	MFCALLSTACK;

	MFRenderer_DeinitModulePlatformSpecific();
}

MF_API int MFRenderer_Begin()
{
	MFCALLSTACK;

	MFDebug_Assert(pCurrentMaterial, "No material set!");
	return pCurrentMaterial->pType->materialCallbacks.pBegin(pCurrentMaterial);
}

MF_API const MFMatrix& MFRenderer_GetMatrix(MFMatrixType type)
{
	MFCALLSTACK;

	return gTransformationMatrices[type];
}

MF_API void MFRenderer_SetMatrix(MFMatrixType type, const MFMatrix &matrix)
{
	MFCALLSTACK;

	gTransformationMatrices[type] = matrix;
}

MF_API void MFRenderer_SetMatrices(const MFMatrix *pMatrices, int numMatrices)
{
	pAnimMats = pMatrices;
	gNumAnimMats = numMatrices;
}

MF_API void MFRenderer_SetBatch(const uint16 *pBatch, int numBonesInBatch)
{
	pCurrentBatch = pBatch;
	gNumBonesInBatch = numBonesInBatch;
}

MF_API uintp MFRenderer_SetRenderStateOverride(uint32 renderState, uintp value)
{
	MFCALLSTACK;

	uintp t = gRenderStates[renderState];
	gRenderStates[renderState] = value;

	return t;
}

MF_API uintp MFRenderer_GetRenderStateOverride(uint32 renderState)
{
	MFCALLSTACK;

	return gRenderStates[renderState];
}
