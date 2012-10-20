module fuji.asset.intmodel;

import fuji.fuji;

enum MFIntModelFormat
{
	Unknown = -1,

	F3D = 0,
	DAE,
	X,
	ASE,
	OBJ,
	MD2,
	MD3,
	MEMD2
}

struct MFIntModel;

extern (C) MFIntModel* MFIntModel_CreateFromFile(const(char*) pFilename);
extern (C) MFIntModel* MFIntModel_CreateFromFileInMemory(const(void*) pMemory, size_t size, MFIntModelFormat format, const(char*) pName);

extern (C) void MFIntModel_Optimise(MFIntModel* pModel);

extern (C) void MFIntModel_CreateRuntimeData(MFIntModel* pModel, void** ppOutput, size_t* pSize, MFPlatform platform);

extern (C) void MFIntModel_Destroy(MFIntModel* pModel);

