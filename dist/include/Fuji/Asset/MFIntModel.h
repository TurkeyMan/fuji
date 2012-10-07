#if !defined(_MFINTMODEL_H)
#define _MFINTMODEL_H

enum MFIntModelFormat
{
	MFIMF_Unknown = -1,

	MFIMF_F3D = 0,
	MFIMF_DAE,
	MFIMF_X,
	MFIMF_ASE,
	MFIMF_OBJ,
	MFIMF_MD2,
	MFIMF_MD3,
	MFIMF_MEMD2,

	MFIMF_Max,
	MFIMF_ForceInt = 0x7FFFFFFF
};

struct MFIntModel;

MF_API MFIntModel *MFIntModel_CreateFromFile(const char *pFilename);
MF_API MFIntModel *MFIntModel_CreateFromFileInMemory(const void *pMemory, size_t size, MFIntModelFormat format, const char *pName);

MF_API void MFIntModel_Optimise(MFIntModel *pModel);

MF_API void MFIntModel_CreateRuntimeData(MFIntModel *pModel, void **ppOutput, size_t *pSize, MFPlatform platform);

MF_API void MFIntModel_Destroy(MFIntModel *pModel);

#endif
