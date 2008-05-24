#if !defined(_MFINTMODEL_H)
#define _MFINTMODEL_H

enum MFIntModelFormat
{
	MFIMF_Unknown = -1,

	MFIMF_F3D = 0,
	MFIMF_ASE,
	MFIMF_OBJ,
	MFIMF_DAE,
	MFIMF_X,
	MFIMF_MD2,
	MFIMF_MD3,
	MFIMF_MEMD2,

	MFIMF_Max,
	MFIMF_ForceInt = 0x7FFFFFFF
};

void MFIntModel_CreateFromFile(const char *pFilename, MFIntModel **ppModel);
void MFIntModel_CreateFromFileInMemory(const void *pMemory, uint32 size, MFIntModel **ppModel, MFIntModelFormat format);

void MFIntModel_Optimise(MFIntModel *pModel);

void MFIntModel_CreateRuntimeData(MFIntModel *pModel, void **ppOutput, uint32 *pSize, MFPlatform platform = MFP_Current);

void MFIntModel_Destroy(MFIntModel *pModel);

#endif
