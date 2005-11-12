#if !defined(_MATERIAL_H)
#define _MATERIAL_H

class MFIni;
struct MFMaterial;
struct MFMaterialCallbacks;
struct MFMaterialParamaterInfo;

// stock (built-in) materials
enum MFStockMaterials
{
	MFMat_White,
	MFMat_Unavailable,
	MFMat_SysLogoSmall,
	MFMat_SysLogoLarge,

	MFMat_USB,
	MFMat_Connected,
	MFMat_Disconnected,
	MFMat_Power,
	MFMat_Charging,

	MFMat_Max,
	MFMat_ForceInt = 0x7FFFFFFF
};

enum MFParamType
{
	MFParamType_Unknown,

	MFParamType_String,
	MFParamType_Float,
	MFParamType_Int,
	MFParamType_Bool,
	MFParamType_Vector3,
	MFParamType_Vector4,
	MFParamType_Matrix,

	MFParamType_Max,
	MFParamType_ForceInt = 0x7FFFFFFF
};

// interface functions

// manage definitions files
int			MFMaterial_AddDefinitionsFile(const char *pName, const char *pFilename);
int			MFMaterial_AddDefinitionsIni(const char *pName, MFIni *pMatDefs);
void		MFMaterial_RemoveDefinitions(const char *pName);

// create/destroy
MFMaterial*	MFMaterial_Create(const char *pName);
int			MFMaterial_Destroy(MFMaterial *pMaterial);

MFMaterial*	MFMaterial_Find(const char *pName);
MFMaterial*	MFMaterial_GetStockMaterial(MFStockMaterials materialIdentifier);

// get/set the current material
void		MFMaterial_SetMaterial(MFMaterial *pMaterial);
MFMaterial*	MFMaterial_GetCurrent();

// material control
int			MFMaterial_GetNumParamaters(MFMaterial *pMaterial);
const char*	MFMaterial_GetParamaterName(MFMaterial *pMaterial, int paramaterIndex);
int			MFMaterial_GetParamaterIndexFromName(MFMaterial *pMaterial, const char *pParameterName);
int			MFMaterial_GetNumParamaterArgs(MFMaterial *pMaterial, int paramaterIndex);
MFParamType	MFMaterial_GetParamaterArgType(MFMaterial *pMaterial, int paramaterIndex, int argIndex);

void		MFMaterial_SetParamater(MFMaterial *pMaterial, int paramaterIndex, int argIndex, uint32 value);
int			MFMaterial_GetParamater(MFMaterial *pMaterial, int paramaterIndex, int argIndex, uint32 *pValue);

// material registration
void		MFMaterial_RegisterMaterialType(const char *pName, const MFMaterialCallbacks *pCallbacks);
void		MFMaterual_UnregisterMaterialType(const char *pName);

// material type registration callbacks
struct MFMaterialCallbacks
{
	int       (*pRegisterMaterial)(void *pPlatformData);
	void      (*pUnregisterMaterial)();

	void      (*pCreateInstance)(MFMaterial *pMaterial);
	void      (*pDestroyInstance)(MFMaterial *pMaterial);

	int       (*pBegin)(MFMaterial *pMaterial);

	void      (*pUpdate)(MFMaterial *pMaterial);

	int       (*pGetNumParams)();
	MFMaterialParamaterInfo* (*pGetParamaterInfo)(int paramaterIndex);
	void      (*pSetParameter)(MFMaterial *pMaterial, int paramaterIndex, int argIndex, uint32 parameter);
	uint32    (*pGetParameter)(MFMaterial *pMaterial, int paramaterIndex, int argIndex);
};


// exposed for writing custom materials
struct MFMaterialParamaterInfo
{
	const char *pParamaterName;

	MFParamType *pArgTypes;
	int numArgs;
};

#endif
