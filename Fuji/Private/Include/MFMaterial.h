#if !defined(_MATERIAL_H)
#define _MATERIAL_H

class MFIni;
struct MFMaterial;
struct MFMaterialCallbacks;
struct MFMaterialParamaterInfo;

// stock (built-in) materials
enum StockMaterials
{
	Mat_White,
	Mat_Unavailable,
	Mat_SysLogoSmall,
	Mat_SysLogoLarge,

	Mat_Max,
	Mat_ForceInt = 0x7FFFFFFF
};

enum ParamType
{
	ParamType_Unknown,

	ParamType_String,
	ParamType_Float,
	ParamType_Int,
	ParamType_Bool,
	ParamType_Vector3,
	ParamType_Vector4,

	ParamType_Max,
	ParamType_ForceInt = 0x7FFFFFFF
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
MFMaterial*	MFMaterial_GetStockMaterial(StockMaterials materialIdentifier);

// get/set the current material
void		MFMaterial_SetMaterial(MFMaterial *pMaterial);
MFMaterial*	MFMaterial_GetCurrent();

// material control
int			MFMaterial_GetNumParamaters(MFMaterial *pMaterial);
const char*	MFMaterial_GetParamaterName(MFMaterial *pMaterial, int paramaterIndex);
int			MFMaterial_GetParamaterIndexFromName(MFMaterial *pMaterial, const char *pParameterName);
int			MFMaterial_GetNumParamaterArgs(MFMaterial *pMaterial, int paramaterIndex);
ParamType	MFMaterial_GetParamaterArgType(MFMaterial *pMaterial, int paramaterIndex, int argIndex);

void		MFMaterial_SetParamater(MFMaterial *pMaterial, int paramaterIndex, int argIndex, uint32 value);
int			MFMaterial_GetParamater(MFMaterial *pMaterial, int paramaterIndex, int argIndex, uint32 *pValue);

// material registration
void		MFMaterial_RegisterMaterialType(const char *pName, const MFMaterialCallbacks *pCallbacks);
void		Materual_UnregisterMaterialType(const char *pName);

// material type registration callbacks
struct MFMaterialCallbacks
{
	int       (*pRegisterMaterial)(void *pPlatformData);
	void      (*pUnregisterMaterial)();

	void      (*pCreateInstance)(MFMaterial *pMaterial);
	void      (*pDestroyInstance)(MFMaterial *pMaterial);

	int       (*pBegin)(MFMaterial *pMaterial);

	void      (*pUpdate)(MFMaterial *pMaterial);

	void      (*pSetParameter)(MFMaterial *pMaterial, int paramaterIndex, int argIndex, uint32 parameter);
	uint32    (*pGetParameter)(MFMaterial *pMaterial, int paramaterIndex, int argIndex);
	int       (*pGetNumParams)();
	MFMaterialParamaterInfo* (*pGetParamaterInfo)(int paramaterIndex);
};

#endif
