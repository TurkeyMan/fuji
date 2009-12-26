/**** Defines ****/

/**** Includes ****/

#include "Fuji.h"
#include "Display.h"
#include "MFTexture_Internal.h"
#include "MFMaterial_Internal.h"
#include "MFFileSystem.h"
#include "MFIni.h"
#include "MFPtrList.h"
#include "MFSystem.h"

#include "MFPrimitive.h"
#include "MFFont.h"
#include "MFInput.h"

#include "SysLogo256.h"
#include "SysLogo64.h"
#if defined(_PSP)
#include "connected.h"
#include "disconnected.h"
#include "power.h"
#include "charging.h"
#include "usb_icon.h"
#endif

/**** internal functions ****/

void		MaterialInternal_Update(MFMaterial *pMaterial);
const char*	MaterialInternal_GetIDString(MFMaterial *pMaterial);

void		MaterialInternal_InitialiseFromDefinition(MFIni *pDefIni, MFMaterial *pMat, const char *pDefinition);


/**** Globals ****/

struct MaterialDefinition
{
	const char *pName;
	MFIni *pIni;
	bool ownsIni;

	MaterialDefinition *pNextDefinition;
};

void MFMaterial_DestroyDefinition(MaterialDefinition *pDefinition);

MFPtrListDL<MaterialDefinition> gMaterialDefList;
MaterialDefinition *pDefinitionRegistry = NULL;

MFPtrList<MFMaterial> gMaterialList;
MFPtrList<MFMaterialType> gMaterialRegistry;

MFMaterial *pCurrentMaterial = NULL;

MFMaterial *pNoneMaterial = NULL;
MFMaterial *pWhiteMaterial = NULL;
MFMaterial *pSysLogoLarge = NULL;
MFMaterial *pSysLogoSmall = NULL;
MFMaterial *pConnected = NULL;
MFMaterial *pDisconnected = NULL;
MFMaterial *pPower = NULL;
MFMaterial *pCharging = NULL;
MFMaterial *pUSB = NULL;

void MFMat_Standard_Register();
void MFMat_Effect_Register();

MaterialBrowser matBrowser;

/**** Functions ****/

void MFMaterial_InitModule()
{
	MFCALLSTACK;

	gMaterialRegistry.Init("Material Registry", gDefaults.material.maxMaterialTypes);
	gMaterialDefList.Init("Material Definitions List", gDefaults.material.maxMaterialDefs);
	gMaterialList.Init("Material List", gDefaults.material.maxMaterials);

	DebugMenu_AddItem("Material Browser", "Fuji Options", &matBrowser);

	MFMat_Standard_Register();
	MFMat_Effect_Register();

	if(MFFileSystem_Exists("Materials.ini") && MFMaterial_AddDefinitionsFile("Materials", "Materials"))
	{
		MFDebug_Warn(3, "Failed to load Materials.ini");
	}

	// create the logo textures from raw data
	MFTexture *pSysLogoLargeTexture = MFTexture_CreateFromRawData("SysLogoLarge", SysLogo256_data, SysLogo256_width, SysLogo256_height, (MFTextureFormat)SysLogo256_format, SysLogo256_flags);
	MFTexture *pSysLogoSmallTexture = MFTexture_CreateFromRawData("SysLogoSmall", SysLogo64_data, SysLogo64_width, SysLogo64_height, (MFTextureFormat)SysLogo64_format, SysLogo64_flags);

	// create standard materials
	pNoneMaterial = MFMaterial_Create("_None");
	pWhiteMaterial = MFMaterial_Create("_White");
	pSysLogoLarge = MFMaterial_Create("SysLogoLarge");
	pSysLogoSmall = MFMaterial_Create("SysLogoSmall");

	// disable backface cullign on the default materials
	size_t zero = 0;
	int cull = MFMaterial_GetParameterIndexFromName(pNoneMaterial, "cullmode");
	if(cull > -1) MFMaterial_SetParameter(pNoneMaterial, cull, 0, &zero);
	cull = MFMaterial_GetParameterIndexFromName(pWhiteMaterial, "cullmode");
	if(cull > -1) MFMaterial_SetParameter(pWhiteMaterial, cull, 0, &zero);

	// release a reference to the logo textures
	MFTexture_Destroy(pSysLogoLargeTexture);
	MFTexture_Destroy(pSysLogoSmallTexture);

#if defined(_PSP)
	// create PSP specific stock materials
	MFTexture *pConnectedTexture = MFTexture_CreateFromRawData("connected", connected_data, connected_width, connected_height, (MFTextureFormat)connected_format, connected_flags);
	MFTexture *pDisconnectedTexture = MFTexture_CreateFromRawData("disconnected", disconnected_data, disconnected_width, disconnected_height, (MFTextureFormat)disconnected_format, disconnected_flags);
	MFTexture *pPowerTexture = MFTexture_CreateFromRawData("power", power_data, power_width, power_height, (MFTextureFormat)power_format, power_flags);
	MFTexture *pChargingTexture = MFTexture_CreateFromRawData("charging", charging_data, charging_width, charging_height, (MFTextureFormat)charging_format, charging_flags);
	MFTexture *pUSBTexture = MFTexture_CreateFromRawData("usb_icon", usb_icon_data, usb_icon_width, usb_icon_height, (MFTextureFormat)usb_icon_format, usb_icon_flags);

	pConnected = MFMaterial_Create("connected");
	pDisconnected = MFMaterial_Create("disconnected");
	pPower = MFMaterial_Create("power");
	pCharging = MFMaterial_Create("charging");
	pUSB = MFMaterial_Create("usb_icon");

	MFTexture_Destroy(pConnectedTexture);
	MFTexture_Destroy(pDisconnectedTexture);
	MFTexture_Destroy(pPowerTexture);
	MFTexture_Destroy(pChargingTexture);
	MFTexture_Destroy(pUSBTexture);
#endif
}

void MFMaterial_DeinitModule()
{
	MFCALLSTACK;

	// destroy stock materials
	MFMaterial_Destroy(pNoneMaterial);
	MFMaterial_Destroy(pWhiteMaterial);
	MFMaterial_Destroy(pSysLogoLarge);
	MFMaterial_Destroy(pSysLogoSmall);

#if defined(_PSP)
	// destroy PSP specific stock materials
	MFMaterial_Destroy(pConnected);
	MFMaterial_Destroy(pDisconnected);
	MFMaterial_Destroy(pPower);
	MFMaterial_Destroy(pCharging);
	MFMaterial_Destroy(pUSB);
#endif

	MaterialDefinition *pDef = pDefinitionRegistry;

	while(pDef)
	{
		MaterialDefinition *pNext = pDef->pNextDefinition;
		MFMaterial_DestroyDefinition(pDef);
		pDef = pNext;
	}

	// list all non-freed materials...
	MFMaterial **ppI = gMaterialList.Begin();
	bool bShowHeader = true;

	while(*ppI)
	{
		if(bShowHeader)
		{
			bShowHeader = false;
			MFDebug_Message("\nUn-freed materials:\n----------------------------------------------------------");
		}

		MFDebug_Message(MFStr("'%s' - x%d", (*ppI)->pName, (*ppI)->refCount));

		(*ppI)->refCount = 1;
		MFMaterial_Destroy(*ppI);

		ppI++;
	}

	MFMaterial_UnregisterMaterialType("Standard");
	MFMaterial_UnregisterMaterialType("Effect");

	gMaterialList.Deinit();
	gMaterialDefList.Deinit();
	gMaterialRegistry.Deinit();
}

void MFMaterial_Update()
{
	MFCALLSTACK;

	MFMaterial **ppMatIterator = gMaterialList.Begin();

	while(ppMatIterator && *ppMatIterator)
	{
		if((*ppMatIterator)->pType->materialCallbacks.pUpdate)
			(*ppMatIterator)->pType->materialCallbacks.pUpdate(*ppMatIterator);

		ppMatIterator++;
	}
}

// interface functions
int MFMaterial_AddDefinitionsFile(const char *pName, const char *pFilename)
{
	MFCALLSTACK;

	MaterialDefinition *pDef = gMaterialDefList.Create();

	pDef->pName = pName;
	pDef->pIni = MFIni::Create(pFilename);
	if (!pDef->pIni)
	{
		gMaterialDefList.Destroy(pDef);
		MFDebug_Warn(2, "Couldnt create material definitions...");
		return 1;
	}

	pDef->ownsIni = true;

	pDef->pNextDefinition = pDefinitionRegistry;
	pDefinitionRegistry = pDef;

	return 0;
}

int MFMaterial_AddDefinitionsIni(const char *pName, MFIni *pMatDefs)
{
	MFCALLSTACK;

	MaterialDefinition *pDef = gMaterialDefList.Create();

	pDef->pName = pName;
	pDef->pIni = pMatDefs;
	pDef->ownsIni = false;

	pDef->pNextDefinition = pDefinitionRegistry;
	pDefinitionRegistry = pDef;

	return 0;
}

void MFMaterial_DestroyDefinition(MaterialDefinition *pDefinition)
{
	MFCALLSTACK;

	if(pDefinition->ownsIni)
	{
		MFIni::Destroy(pDefinition->pIni);
		pDefinition->pIni = NULL;
	}

	gMaterialDefList.Destroy(pDefinition);
}

void MFMaterial_RemoveDefinitions(const char *pName)
{
	MFCALLSTACK;

	MaterialDefinition *pDef = pDefinitionRegistry;

	if(!pDef)
		return;

	if(!MFString_Compare(pDef->pName, pName))
	{
		pDefinitionRegistry = pDef->pNextDefinition;
		MFMaterial_DestroyDefinition(pDef);
		return;
	}

	while(pDef->pNextDefinition)
	{
		if(!MFString_Compare(pDef->pNextDefinition->pName, pName))
		{
			MaterialDefinition *pDestroy = pDef->pNextDefinition;
			pDef->pNextDefinition = pDef->pNextDefinition->pNextDefinition;

			MFMaterial_DestroyDefinition(pDestroy);
			return;
		}

		pDef = pDef->pNextDefinition;
	}
}

void MFMaterial_RegisterMaterialType(const char *pName, const MFMaterialCallbacks *pCallbacks)
{
	MFCALLSTACK;

	MFMaterialType *pMatType;
	pMatType = (MFMaterialType*)MFHeap_Alloc(sizeof(MFMaterialType) + MFString_Length(pName) + 1);

	pMatType->pTypeName = (char*)&pMatType[1];
	MFString_Copy(pMatType->pTypeName, pName);

	MFDebug_Assert(pCallbacks->pBegin, "Material must supply Begin() callback.");

	MFCopyMemory(&pMatType->materialCallbacks, pCallbacks, sizeof(MFMaterialCallbacks));

	gMaterialRegistry.Create(pMatType);

	pCallbacks->pRegisterMaterial(NULL);
}

void MFMaterial_UnregisterMaterialType(const char *pName)
{
	MFCALLSTACK;

	MFMaterialType *pMatType = MaterialInternal_GetMaterialType(pName);

	MFDebug_Assert(pMatType, MFStr("Material type '%s' doesn't exist!", pName));

	if(pMatType)
	{
		pMatType->materialCallbacks.pUnregisterMaterial();
		gMaterialRegistry.Destroy(pMatType);
		MFHeap_Free(pMatType);
	}
}

MFMaterialType *MaterialInternal_GetMaterialType(const char *pTypeName)
{
	MFCALLSTACK;

	MFMaterialType **ppIterator = gMaterialRegistry.Begin();

	while(*ppIterator)
	{
		if(!MFString_CaseCmp(pTypeName, (*ppIterator)->pTypeName)) return *ppIterator;

		ppIterator++;
	}

	return NULL;
}

MFMaterial* MFMaterial_Create(const char *pName)
{
	MFCALLSTACK;

	MFMaterial *pMat = MFMaterial_Find(pName);

	if(!pMat)
	{
		pMat = (MFMaterial*)MFHeap_Alloc(sizeof(MFMaterial) + MFString_Length(pName) + 1);
		MFZeroMemory(pMat, sizeof(MFMaterial));

		pMat->pName = (char*)&pMat[1];
		MFString_Copy(pMat->pName, pName);

		gMaterialList.Create(pMat);

		MaterialDefinition *pDef = pDefinitionRegistry;
		while(pDef)
		{
			MFIniLine *pLine = pDef->pIni->GetFirstLine()->FindEntry("section",pName);
			if (pLine)
			{
				MaterialInternal_InitialiseFromDefinition(pDef->pIni, pMat, pName);
				break;
			}

			pDef = pDef->pNextDefinition;
		}

		if(!pDef)
		{
			// assign material type
			pMat->pType = MaterialInternal_GetMaterialType("Standard");
			pMat->pType->materialCallbacks.pCreateInstance(pMat);

			// set diffuse map parameter
			int index = MFMaterial_GetParameterIndexFromName(pMat, "diffusemap");
			if(index > -1)
				MFMaterial_SetParameter(pMat, index, 0, pName);
		}
	}

	pMat->refCount++;

	return pMat;
}

int MFMaterial_Destroy(MFMaterial *pMaterial)
{
	MFCALLSTACK;

	--pMaterial->refCount;

	if(!pMaterial->refCount)
	{
		pMaterial->pType->materialCallbacks.pDestroyInstance(pMaterial);

		gMaterialList.Destroy(pMaterial);

		return 0;
	}

	return pMaterial->refCount;
}

MFMaterial* MFMaterial_Find(const char *pName)
{
	MFCALLSTACK;

	MFMaterial **ppIterator = gMaterialList.Begin();

	while(*ppIterator)
	{
		if(!MFString_CaseCmp(pName, (*ppIterator)->pName)) return *ppIterator;

		ppIterator++;
	}

	return NULL;
}

MFMaterial* MFMaterial_GetCurrent()
{
	return pCurrentMaterial;
}

const char *MFMaterial_GetMaterialName(MFMaterial *pMaterial)
{
	return pMaterial->pName;
}

void MFMaterial_SetMaterial(const MFMaterial *pMaterial)
{
	MFCALLSTACK;

	if(!pMaterial)
		pMaterial = MFMaterial_GetStockMaterial(MFMat_White);

	pCurrentMaterial = (MFMaterial*)pMaterial;
}

MFMaterial* MFMaterial_GetStockMaterial(MFStockMaterials materialIdentifier)
{
	MFCALLSTACK;

	switch(materialIdentifier)
	{
		case MFMat_White:
			return pWhiteMaterial;
		case MFMat_Unavailable:
			return pNoneMaterial;
		case MFMat_SysLogoSmall:
			return pSysLogoSmall;
		case MFMat_SysLogoLarge:
			return pSysLogoLarge;
		case MFMat_USB:
			return pUSB;
		case MFMat_Connected:
			return pConnected;
		case MFMat_Disconnected:
			return pDisconnected;
		case MFMat_Power:
			return pPower;
		case MFMat_Charging:
			return pCharging;
		default:
			MFDebug_Assert(false, "Invalid Stock Material");
	}

	return NULL;
}


// internal functions
const char* MaterialInternal_GetIDString()
{
	MFCALLSTACK;
/*
	char *id = &stringBuffer[stringBufferOffset];
	*id = NULL;

	for(int a=0; a<32; a++)
	{
		if((flags>>a)&1) MFString_Cat(id, matDesc[a]);
	}

	stringBufferOffset += MFString_Length(id) + 1;
*/
	return NULL;
}

void MaterialInternal_InitialiseFromDefinition(MFIni *pDefIni, MFMaterial *pMat, const char *pDefinition)
{
	MFCALLSTACK;

	MFIniLine *pLine = pDefIni->GetFirstLine()->FindEntry("section", pDefinition);

	if(pLine)
	{
		pLine = pLine->Sub();

		if(pLine && pLine->IsString(0, "type"))
		{
			pMat->pType = MaterialInternal_GetMaterialType(pLine->GetString(1));

			if(!pMat->pType)
				pMat->pType = MaterialInternal_GetMaterialType("Standard");

			pLine = pLine->Next();
		}
		else
		{
			pMat->pType = MaterialInternal_GetMaterialType("Standard");
		}

		if(pMat->pType->materialCallbacks.pCreateInstance)
			pMat->pType->materialCallbacks.pCreateInstance(pMat);

		while(pLine)
		{
			if(pLine->IsString(0,"type"))
			{
				MFDebug_Warn(2, MFStr("'type' MUST be the first parameter in a material definition... Ignored, Using type '%s'.", pMat->pType->pTypeName));
			}
			else if(pLine->IsString(0,"alias"))
			{
				MFDebug_Warn(2, "'alias' MUST be the first parameter in a material definition... Ignored.");

MFDebug_Assert(false, "Fix Me!!!");
//				const char *pAlias = pLine->GetString(1);
//				MaterialInternal_InitialiseFromDefinition(pDefIni, pMat, pAlias);
			}
			else
			{
				const char *pParam = pLine->GetString(0);

				int paramIndex = MFMaterial_GetParameterIndexFromName(pMat, pParam);
				int numArgs = MFMaterial_GetNumParameterArgs(pMat, paramIndex);

				for(int a=0; a<numArgs; a++)
				{
					MFParamType pt = MFMaterial_GetParameterArgType(pMat, paramIndex, a);

					switch(pt)
					{
						case MFParamType_String:
						{
							const char *pString = pLine->GetString(a+1);
							MFMaterial_SetParameter(pMat, paramIndex, a, pString);
							break;
						}

						case MFParamType_Float:
						{
							float value = pLine->GetFloat(a+1);
							MFMaterial_SetParameter(pMat, paramIndex, a, &value);
							break;
						}

						case MFParamType_Int:
						{
							int value = pLine->GetStringCount() > 1 ? pLine->GetInt(a+1) : 1;
							MFMaterial_SetParameterI(pMat, paramIndex, a, value);
							break;
						}

						case MFParamType_Bool:
						{
							bool value = pLine->GetStringCount() > 1 ? pLine->GetBool(a+1) : true;
							MFMaterial_SetParameterI(pMat, paramIndex, a, value ? 1 : 0);
							break;
						}

						case MFParamType_Vector3:
						{
							MFVector vector = pLine->GetVector3(a+1);
							MFMaterial_SetParameter(pMat, paramIndex, a, &vector);
							break;
						}

						case MFParamType_Vector4:
						{
							MFVector vector = pLine->GetVector4(a+1);
							MFMaterial_SetParameter(pMat, paramIndex, a, &vector);
							break;
						}

						case MFParamType_Matrix:
						{
							MFDebug_Assert(false, "Cant read a matrix from an ini file... yet...");
							break;
						}

						default:
							MFDebug_Assert(false, "Unknown parameter type..");
					}
				}
			}

			pLine = pLine->Next();
		}
	}
}

int MFMaterial_GetNumParameters(MFMaterial *pMaterial)
{
	MFCALLSTACK;

	return pMaterial->pType->materialCallbacks.pGetNumParams ? pMaterial->pType->materialCallbacks.pGetNumParams() : 0;
}

const char* MFMaterial_GetParameterName(MFMaterial *pMaterial, int parameterIndex)
{
	MFCALLSTACK;

	MFMaterialParameterInfo *pInfo;

	MFDebug_Assert(pMaterial->pType->materialCallbacks.pGetParameterInfo, "Material does not supply a GetParameterInfo() function.");
	pInfo = (MFMaterialParameterInfo*)pMaterial->pType->materialCallbacks.pGetParameterInfo(parameterIndex);

	return pInfo->pParameterName;
}

int MFMaterial_GetParameterIndexFromName(MFMaterial *pMaterial, const char *pParameterName)
{
	MFCALLSTACK;

	MFMaterialParameterInfo *pInfo;

	int numParams = MFMaterial_GetNumParameters(pMaterial);

	for(int a=0; a<numParams; a++)
	{
		MFDebug_Assert(pMaterial->pType->materialCallbacks.pGetParameterInfo, "Material does not supply a GetParameterInfo() function.");
		pInfo = (MFMaterialParameterInfo*)pMaterial->pType->materialCallbacks.pGetParameterInfo(a);

		if(!MFString_CaseCmp(pInfo->pParameterName, pParameterName))
			return a;
	}

	return -1;
}

int MFMaterial_GetNumParameterArgs(MFMaterial *pMaterial, int parameterIndex)
{
	MFCALLSTACK;

	MFMaterialParameterInfo *pInfo;

	MFDebug_Assert(pMaterial->pType->materialCallbacks.pGetParameterInfo, "Material does not supply a GetParameterInfo() function.");
	pInfo = (MFMaterialParameterInfo*)pMaterial->pType->materialCallbacks.pGetParameterInfo(parameterIndex);

	return pInfo->numArgs;
}

MFParamType MFMaterial_GetParameterArgType(MFMaterial *pMaterial, int parameterIndex, int argIndex)
{
	MFCALLSTACK;

	MFMaterialParameterInfo *pInfo;

	MFDebug_Assert(pMaterial->pType->materialCallbacks.pGetParameterInfo, "Material does not supply a GetParameterInfo() function.");
	pInfo = (MFMaterialParameterInfo*)pMaterial->pType->materialCallbacks.pGetParameterInfo(parameterIndex);

	return pInfo->pArgTypes[argIndex];
}

void MFMaterial_SetParameter(MFMaterial *pMaterial, int parameterIndex, int argIndex, const void *pValue)
{
	MFCALLSTACK;

	MFDebug_Assert(pMaterial->pType->materialCallbacks.pSetParameter, "Material does not supply a SetParameter() function.");
	pMaterial->pType->materialCallbacks.pSetParameter(pMaterial, parameterIndex, argIndex, pValue);
}

int MFMaterial_GetParameter(MFMaterial *pMaterial, int parameterIndex, int argIndex, void *pValue)
{
	MFCALLSTACK;

	void *pT = pValue ? pValue : &pT;

	MFDebug_Assert(pMaterial->pType->materialCallbacks.pGetParameter, "Material does not supply a GetParameter() function.");
	return pMaterial->pType->materialCallbacks.pGetParameter(pMaterial, parameterIndex, argIndex, pT);
}


// material browser
MaterialBrowser::MaterialBrowser()
{
	selection = 0;
	type = MenuType_TextureBrowser;
}

void MaterialBrowser::Draw()
{

}

void MaterialBrowser::Update()
{
	if(MFInput_WasPressed(Button_XB_Y, IDD_Gamepad))
		pCurrentMenu = pParent;
}

#define TEX_SIZE 64.0f
float MaterialBrowser::ListDraw(bool selected, const MFVector &_pos, float maxWidth)
{
	MFVector pos = _pos;

	MFMaterial **i;
	i = gMaterialList.Begin();

	for(int a=0; a<selection; a++) i++;

	MFMaterial *pMaterial = *i;

	MFFont_DrawText(MFFont_GetDebugFont(), pos+MakeVector(0.0f, ((TEX_SIZE+8.0f)*0.5f)-(MENU_FONT_HEIGHT*0.5f)-MENU_FONT_HEIGHT, 0.0f), MENU_FONT_HEIGHT, selected ? MakeVector(1,1,0,1) : MFVector::one, MFStr("%s:", name));
	MFFont_DrawText(MFFont_GetDebugFont(), pos+MakeVector(10.0f, ((TEX_SIZE+8.0f)*0.5f)-(MENU_FONT_HEIGHT*0.5f), 0.0f), MENU_FONT_HEIGHT, selected ? MakeVector(1,1,0,1) : MFVector::one, MFStr("%s", pMaterial->pName));
	MFFont_DrawText(MFFont_GetDebugFont(), pos+MakeVector(10.0f, ((TEX_SIZE+8.0f)*0.5f)-(MENU_FONT_HEIGHT*0.5f)+MENU_FONT_HEIGHT, 0.0f), MENU_FONT_HEIGHT, selected ? MakeVector(1,1,0,1) : MFVector::one, MFStr("Type: %s Refs: %d", pMaterial->pType->pTypeName, pMaterial->refCount));

	pos += MakeVector(maxWidth - (TEX_SIZE + 4.0f + 5.0f), 2.0f, 0.0f);

	MFPrimitive(PT_TriStrip|PT_Untextured);

	MFBegin(4);
	MFSetColour(MFVector::white);
	MFSetPosition(pos);
	MFSetPosition(pos + MakeVector(TEX_SIZE + 4.0f, 0.0f, 0.0f));
	MFSetPosition(pos + MakeVector(0.0f, TEX_SIZE + 4.0f, 0.0f));
	MFSetPosition(pos + MakeVector(TEX_SIZE + 4.0f, TEX_SIZE + 4.0f, 0.0f));
	MFEnd();

	pos += MakeVector(2.0f, 2.0f, 0.0f);

	const int numSquares = 7;
	for(int a=0; a<numSquares; a++)
	{
		for(int b=0; b<numSquares; b++)
		{
			float x, y, w, h;
			w = TEX_SIZE/(float)numSquares;
			h = TEX_SIZE/(float)numSquares;
			x = pos.x + (float)b*w;
			y = pos.y + (float)a*h;

			MFBegin(4);
			MFSetColour(((a+b)&1) ? MakeVector(.75f, .75f, .75f, 1.f) : MakeVector(.2f, .2f, .2f, 1.f));
			MFSetPosition(x,y,0);
			MFSetPosition(x+w,y,0);
			MFSetPosition(x,y+h,0);
			MFSetPosition(x+w,y+h,0);
			MFEnd();
		}
	}

	MFMaterial_SetMaterial(pMaterial);

	MFPrimitive(PT_TriStrip);

	MFBegin(4);
	MFSetColour(MFVector::white);
	MFSetTexCoord1(0.0f,0.0f);
	MFSetPosition(pos);
	MFSetTexCoord1(1.0f,0.0f);
	MFSetPosition(pos + MakeVector(TEX_SIZE, 0.0f, 0.0f));
	MFSetTexCoord1(0.0f,1.0f);
	MFSetPosition(pos + MakeVector(0.0f, TEX_SIZE, 0.0f));
	MFSetTexCoord1(1.0f,1.0f);
	MFSetPosition(pos + MakeVector(TEX_SIZE, TEX_SIZE, 0.0f));
	MFEnd();

	return TEX_SIZE + 8.0f;
}

void MaterialBrowser::ListUpdate(bool selected)
{
	if(selected)
	{
		int texCount = gMaterialList.GetLength();

		if(MFInput_WasPressed(Button_DLeft, IDD_Gamepad))
		{
			selection = selection <= 0 ? texCount-1 : selection-1;

			if(pCallback)
				pCallback(this, pUserData);
		}
		else if(MFInput_WasPressed(Button_DRight, IDD_Gamepad))
		{
			selection = selection >= texCount-1 ? 0 : selection+1;

			if(pCallback)
				pCallback(this, pUserData);
		}
	}
}

MFVector MaterialBrowser::GetDimensions(float maxWidth)
{
	return MakeVector(maxWidth, TEX_SIZE + 8.0f, 0.0f);
}

